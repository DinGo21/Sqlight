#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "node.h"
#include "globals.h"
#include "pager.h"

uint32_t *
node_leaf_num_cells(void *node)
{
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

void *
node_leaf_cell(void *node, uint32_t cell_num)
{
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *
node_leaf_key(void *node, uint32_t cell_num)
{
    return node_leaf_cell(node, cell_num);
}

void *
node_leaf_value(void *node, uint32_t cell_num)
{
    return node_leaf_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

void
node_leaf_initialize(void *node)
{
    *node_leaf_num_cells(node) = 0;
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
    num_cells = *node_leaf_num_cells(node);
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
            memcpy(node_leaf_cell(node, i), node_leaf_cell(node, i - 1),
                    LEAF_NODE_CELL_SIZE);
        }
    }
    *node_leaf_num_cells(node) += 1;
    *node_leaf_key(node, cursor->cell_num) = key;
    row_serialize(value, node_leaf_value(node, cursor->cell_num));
    return 0;
}

