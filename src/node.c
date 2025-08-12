#include <string.h>
#include <unistd.h>
#include "node.h"
#include "cursor.h"
#include "globals.h"
#include "pager.h"
#include "row.h"
#include "table.h"

uint32_t *
node_leaf_move_to_num_cells(void *node)
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
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

uint32_t
node_get_max_key(void *node)
{
    switch(node_get_type(node))
    {
        case NODE_INTERNAL:
            return *node_internal_move_to_key(node, 
                                    *node_internal_move_to_num_keys(node) - 1);
        case NODE_LEAF:
            return *node_leaf_move_to_key(node, 
                                        *node_leaf_move_to_num_cells(node) - 1);
    }
}

int
node_is_root(void *node)
{
    return *((uint8_t *)(node + IS_ROOT_OFFSET));
}

void
node_set_root(void *node, int is_root)
{
    *((uint8_t *)(node + IS_ROOT_OFFSET)) = is_root != 0;
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
    left_child_max_key = node_get_max_key(left_child);
    memcpy(left_child, root, PAGE_SIZE);
    node_set_root(left_child, 0);
    node_internal_init(root);
    node_set_root(root, 1);
    *node_internal_move_to_num_keys(root) = 1;
    *node_internal_move_to_child(root, 0) = left_child_page_num;
    *node_internal_move_to_key(root, 0) = left_child_max_key;
    *node_internal_move_to_right_child(root) = right_child_page_num;
    return 0;
}

void
node_leaf_init(void *node)
{
    node_set_type(node, NODE_LEAF);
    node_set_root(node, 0);
    *node_leaf_move_to_num_cells(node) = 0;
}

static void
node_leaf_insert_into_split(cursor_t *cursor, row_t *value, void *old_node,
                            void *new_node)
{
    uint32_t    i;
    uint32_t    index_within_node;
    void        *destination_node;
    void        *destination;

    i = LEAF_NODE_MAX_CELLS;
    while (i >= 0)
    {
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT)
            destination_node = new_node;
        else
            destination_node = old_node;
        index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        destination = node_leaf_move_to_cell(destination_node, 
                                            index_within_node);
        if (i == cursor->cell_num)
            row_serialize(value, destination);
        else if (i >= cursor->cell_num)
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
node_leaf_split(cursor_t *cursor, const uint32_t key, row_t *value)
{
    void        *old_node;
    void        *new_node;
    uint32_t    new_page_num;

    (void)key;
    old_node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (old_node == NULL)
        return -1;
    new_page_num = pager_get_unused_page_num(cursor->table->pager);
    new_node = pager_get_page(cursor->table->pager, new_page_num);
    if (new_node == NULL)
        return -1;
    node_leaf_init(new_node);
    node_leaf_insert_into_split(cursor, value, old_node, new_node);
    *(node_leaf_move_to_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(node_leaf_move_to_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;
    if (node_is_root(old_node))
        return node_create_root(cursor->table, new_page_num);
    //TODO: update parent.
    return -1;
}

int
node_leaf_insert(cursor_t *cursor, const uint32_t key, row_t *value)
{
    void        *node;
    uint32_t    num_cells;
    uint32_t    i;

    node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (node == NULL)
        return -1;
    num_cells = *node_leaf_move_to_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS)
    {
        node_leaf_split(cursor, key, value);
        return 0;
    }
    if (cursor->cell_num < num_cells)
    {
        i = num_cells;
        while (i > cursor->cell_num)
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


uint32_t
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

void
node_internal_init(void *node)
{
    node_set_type(node, NODE_INTERNAL);
    node_set_root(node, 0);
    *node_internal_move_to_num_keys(node) = 0;
}

uint32_t
node_internal_find_key(void *node, uint32_t key)
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

uint32_t *
node_internal_move_to_cell(void *node, uint32_t cell_num)
{
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t *
node_internal_move_to_child(void *node, uint32_t child_num)
{
    uint32_t    num_keys;
    
    num_keys = *node_internal_move_to_num_keys(node);
    if (child_num > num_keys)
        //TODO: handle errors in case of returning NULL.
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

