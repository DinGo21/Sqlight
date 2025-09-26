#include "node.h"
#include "cursor.h"
#include "globals.h"
#include "pager.h"
#include "row.h"
#include "table.h"

#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>


node_type_t
node_get_type(const void *node)
{
    return *((uint8_t *)(node + NODE_TYPE_OFFSET));
}

void
node_set_type(void *node, node_type_t type)
{
    *((uint8_t *)(node + NODE_TYPE_OFFSET)) = type;
}

uint32_t *
node_move_to_parent(void *node)
{
    return node + PARENT_POINTER_OFFSET;
}

uint32_t
node_get_max_key(pager_t *pager, void *node)
{
    uint32_t    num;

    (void)pager;
    switch(node_get_type(node))
    {
        case NODE_INTERNAL:
            num = *node_internal_move_to_num_keys(node) - 1;
            return *node_internal_move_to_key(node, num);
        case NODE_LEAF:
            num = *node_leaf_move_to_num_cells(node) - 1;
            return *node_leaf_move_to_key(node, num);
    }
}

uint8_t
node_is_root(void *node)
{
    return *((uint8_t *)(node + IS_ROOT_OFFSET));
}

void
node_set_root(void *node, uint8_t is_root)
{
    *((uint8_t *)(node + IS_ROOT_OFFSET)) = is_root;
}

int
node_create_root(table_t *table, uint32_t right_child_page_num)
{
    void        *root;
    void        *right_child;
    void        *left_child;
    uint32_t    left_child_page_num;
    uint32_t    left_child_max_key;

    root = pager_get_page(table->pager, table->root_page_num);
    if (root == NULL)
        return -1;
    right_child = pager_get_page(table->pager, right_child_page_num);
    if (right_child == NULL)
        return -1;
    left_child_page_num = pager_get_unused_page_num(table->pager);
    left_child = pager_get_page(table->pager, left_child_page_num);
    if (left_child == NULL)
        return -1;
    memcpy(left_child, root, PAGE_SIZE);
    node_set_root(left_child, 0);
    node_internal_init(root);
    node_set_root(root, 1);
    *node_internal_move_to_num_keys(root) = 1;
    *node_internal_move_to_child(root, 0) = left_child_page_num;
    left_child_max_key = node_get_max_key(table->pager, left_child);
    *node_internal_move_to_key(root, 0) = left_child_max_key;
    *node_internal_move_to_right_child(root) = right_child_page_num;
    *node_move_to_parent(left_child) = table->root_page_num;
    *node_move_to_parent(right_child) = table->root_page_num;
    return 0;
}

void
node_leaf_init(void *node)
{
    node_set_type(node, NODE_LEAF);
    node_set_root(node, 0);
    *node_leaf_move_to_num_cells(node) = 0;
    *node_leaf_move_to_next_leaf(node) = 0;
}

static void
node_leaf_insert_into_split(cursor_t *cursor, row_t *value, const uint32_t key,
                            void *old_node, void *new_node)
{
    uint32_t    index_within_node;
    void        *destination_node;
    void        *destination;

    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--)
    {

        if (i >= (int32_t)LEAF_NODE_LEFT_SPLIT_COUNT)
            destination_node = new_node;
        else
            destination_node = old_node;
        index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        destination = node_leaf_move_to_cell(destination_node, 
                                            index_within_node);
        if (i == (int32_t)cursor->cell_num)
        {
            *node_leaf_move_to_key(destination_node, index_within_node) = key;
            row_serialize(value, node_leaf_move_to_value(destination_node,
                                                        index_within_node));
        }
        else if (i > (int32_t)cursor->cell_num)
        {
            memcpy(destination, node_leaf_move_to_cell(old_node, i - 1),
                    LEAF_NODE_CELL_SIZE);
        }
        else
        {
            memcpy(destination, node_leaf_move_to_cell(old_node, i),
                    LEAF_NODE_CELL_SIZE);
        }
    }
}

static int
node_update_parent(table_t *table, const uint32_t old_max,
                    const uint32_t new_page_num, void *old_node)
{
    uint32_t    new_max;
    uint32_t    parent_page_num;
    void        *parent_node;

    parent_page_num = *node_move_to_parent(old_node);
    new_max = node_get_max_key(table->pager, old_node);
    parent_node = pager_get_page(table->pager, parent_page_num);
    if (parent_node == NULL)
        return -1;
    node_internal_update_key(parent_node, old_max, new_max);
    if (node_internal_insert(table, parent_page_num, new_page_num) < 0)
        return -1;
    return 0;
}

static int
node_leaf_split(cursor_t *cursor, const uint32_t key, row_t *value)
{
    void        *old_node;
    void        *new_node;
    uint32_t    old_max;
    uint32_t    new_page_num;

    old_node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (old_node == NULL)
        return -1;
    old_max = node_get_max_key(cursor->table->pager, old_node);
    new_page_num = pager_get_unused_page_num(cursor->table->pager);
    new_node = pager_get_page(cursor->table->pager, new_page_num);
    if (new_node == NULL)
        return -1;
    node_leaf_init(new_node);
    *node_move_to_parent(new_node) = *node_move_to_parent(old_node);
    node_leaf_insert_into_split(cursor, value, key, old_node, new_node);
    *node_leaf_move_to_num_cells(old_node) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *node_leaf_move_to_num_cells(new_node) = LEAF_NODE_RIGHT_SPLIT_COUNT;
    *node_leaf_move_to_next_leaf(new_node) = *node_leaf_move_to_next_leaf(
                                                                    old_node);
    *node_leaf_move_to_next_leaf(old_node) = new_page_num;
    if (node_is_root(old_node))
        return node_create_root(cursor->table, new_page_num);
    return node_update_parent(cursor->table, old_max, new_page_num, old_node);
}
int
node_leaf_insert(cursor_t *cursor, const uint32_t key, row_t *value)
{
    void        *node;
    uint32_t    num_cells;

    node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (node == NULL)
        return -1;
    num_cells = *node_leaf_move_to_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
        return node_leaf_split(cursor, key, value);
    }
    if (cursor->cell_num < num_cells)
    {
        for (uint32_t i = num_cells; i > cursor->cell_num; i++)
        {
            memcpy(node_leaf_move_to_cell(node, i),
                    node_leaf_move_to_cell(node, i - 1), LEAF_NODE_CELL_SIZE);
        }
    }
    *node_leaf_move_to_num_cells(node) += 1;
    *node_leaf_move_to_key(node, cursor->cell_num) = key;
    row_serialize(value, node_leaf_move_to_value(node, cursor->cell_num));
    return 0;
}

static uint32_t
node_leaf_find_cell_num(void *node, const uint32_t key)
{
    uint32_t    min_index;
    uint32_t    past_max_index;
    uint32_t    index;
    uint32_t    key_at_index;
    
    min_index = 0;
    past_max_index = *node_leaf_move_to_num_cells(node);
    while (past_max_index != min_index)
    {
        index = (min_index + past_max_index) / 2;
        key_at_index = *node_leaf_move_to_key(node, index);
        if (key == key_at_index)
            return index;
        if (key < key_at_index)
            past_max_index = index;
        else
            min_index = index + 1;
    }
    return min_index;
}

int
node_leaf_find(cursor_t *cursor, table_t *table, const uint32_t page_num,
                const uint32_t key)
{
    void    *node;

    node = pager_get_page(table->pager, page_num);
    if (node == NULL)
        return -1;
    cursor->page_num = page_num;
    cursor->cell_num = node_leaf_find_cell_num(node, key);
    return 0;
}

uint32_t *
node_leaf_move_to_num_cells(void *node)
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t *
node_leaf_move_to_next_leaf(void *node)
{
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

void *
node_leaf_move_to_cell(void *node, const uint32_t cell_num)
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *
node_leaf_move_to_key(void *node, const uint32_t cell_num)
{
    return node_leaf_move_to_cell(node, cell_num);
}

void *
node_leaf_move_to_value(void *node, const uint32_t cell_num)
{
    return node_leaf_move_to_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void
node_internal_init(void *node)
{
    node_set_type(node, NODE_INTERNAL);
    node_set_root(node, 0);
    *node_internal_move_to_num_keys(node) = 0;
}

uint32_t
node_internal_find_child(void *node, const uint32_t key)
{
    uint32_t    min_index;
    uint32_t    max_index;
    uint32_t    index;
    uint32_t    key_to_right;

    min_index = 0;
    max_index = *node_internal_move_to_num_keys(node);
    while (min_index != max_index)
    {
        index = (min_index + max_index) / 2;
        key_to_right = *node_internal_move_to_key(node, index);
        if (key_to_right >= key)
            max_index = index;
        else
            min_index = index + 1;
    }
    return *node_internal_move_to_child(node, min_index);
}

int
node_internal_find(cursor_t *cursor, table_t *table, const uint32_t page_num,
                    const uint32_t key)
{
    uint32_t    child_num;
    void        *node;
    void        *child;

    node = pager_get_page(table->pager, page_num);
    if (node == NULL)
        return -1;
    child_num = node_internal_find_child(node, key);
    child = pager_get_page(table->pager, child_num);
    if (child == NULL)
        return -1;
    switch (node_get_type(child))
    {
        case NODE_LEAF:
            return node_leaf_find(cursor, table, child_num, key);
        case NODE_INTERNAL:
            return node_internal_find(cursor, table, child_num, key);
    }
}

static int
node_internal_insert_into_parent(table_t *table, void *parent,
                                const uint32_t index,
                                const uint32_t original_num_keys,
                                const uint32_t child_page_num,
                                const uint32_t child_max_key)
{
    void        *right_child;
    uint32_t    right_child_page_num;

    right_child_page_num = *node_internal_move_to_right_child(parent);
    right_child = pager_get_page(table->pager, right_child_page_num);
    if (right_child == NULL)
        return -1;
    if (child_max_key >= node_get_max_key(table->pager, right_child))
    {
        *node_internal_move_to_child(parent, original_num_keys) = 
                                                        right_child_page_num;
        *node_internal_move_to_key(parent, original_num_keys) =
                                    node_get_max_key(table->pager, right_child);
        *node_internal_move_to_right_child(parent) = child_page_num;
        return 0;
    }
    for (uint32_t i = original_num_keys; i > index; i--)
    {
       memcpy(node_internal_move_to_cell(parent, i),
               node_internal_move_to_cell(parent, i - 1),
               INTERNAL_NODE_CELL_SIZE);
    }
    return 0;
}

int
node_internal_insert(table_t *table, uint32_t parent_page_num,
                    uint32_t child_page_num)
{
    void        *parent;
    void        *child;
    uint32_t    child_max_key;
    uint32_t    index;
    uint32_t    original_num_keys;

    parent = pager_get_page(table->pager, parent_page_num);
    if (parent == NULL)
        return -1;
    child = pager_get_page(table->pager, child_page_num);
    if (child == NULL)
        return -1;
    child_max_key = node_get_max_key(table->pager, child);
    index = node_internal_find_child(parent, child_max_key);
    original_num_keys = *node_internal_move_to_num_keys(parent);
    *node_internal_move_to_num_keys(parent) = original_num_keys + 1;
    if (original_num_keys >= INTERNAL_NODE_MAX_CELLS)
        return -1;
    if (node_internal_insert_into_parent(table, parent, index,
        original_num_keys, child_page_num, child_max_key) < 0)
        return -1;
    *node_internal_move_to_child(parent, index) = child_page_num;
    *node_internal_move_to_key(parent, index) = child_max_key;
    return 0;
}

uint32_t *
node_internal_move_to_num_keys(void *node)
{
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t *
node_internal_move_to_right_child(void *node)
{
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

void *
node_internal_move_to_cell(void *node, uint32_t cell_num)
{
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num *
            INTERNAL_NODE_CELL_SIZE;
}

uint32_t *
node_internal_move_to_child(void *node, uint32_t child_num)
{
    uint32_t    num_keys;
    
    num_keys = *node_internal_move_to_num_keys(node);
    //TODO: handle errors in case of returning NULL.
    if (child_num > num_keys)
        return NULL;
    if (child_num == num_keys)
        return node_internal_move_to_right_child(node);
    return node_internal_move_to_cell(node, child_num);
}

uint32_t *
node_internal_move_to_key(void *node, uint32_t key_num)
{
    return node_internal_move_to_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

void
node_internal_update_key(void *node, uint32_t old_key, uint32_t new_key)
{
    uint32_t    old_child_index;

    old_child_index = node_internal_find_child(node, old_key);
    *node_internal_move_to_key(node, old_child_index) = new_key;
}

