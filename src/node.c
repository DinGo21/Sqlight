#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "node.h"
#include "cursor.h"
#include "globals.h"
#include "pager.h"
#include "table.h"

uint32_t *
node_leaf_move_to_num_cells(void *node)
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *
node_leaf_move_to_cell(void *node, uint32_t cell_num)
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *
node_leaf_move_to_key(void *node, uint32_t cell_num)
{
    return node_leaf_move_to_cell(node, cell_num);
}

void *
node_leaf_move_to_value(void *node, uint32_t cell_num)
{
    return node_leaf_move_to_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

node_type_t
node_get_type(void *node)
{
    return *((uint8_t *)(node + NODE_TYPE_OFFSET));
}

void
node_set_type(void *node, node_type_t type)
{
    *((uint8_t *)(node + NODE_TYPE_OFFSET)) = type;
}

void
node_leaf_init(void *node)
{
    node_set_type(node, NODE_LEAF);
    *node_leaf_move_to_num_cells(node) = 0;
}

int
node_leaf_insert(cursor_t *cursor, uint32_t key, row_t *value)
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
        //TODO: Adding enum to print error.
        printf("Need to implement splitting\n");
        return -1;
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
node_leaf_find_cell_num(void *node, uint32_t key)
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

