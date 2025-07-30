#ifndef NODE_H
# define NODE_H

#include "cursor.h"
#include "row.h"
#include <stdint.h>

typedef enum node_type_e
{
    NODE_INTERNAL,
    NODE_LEAF
}   node_type_t;

uint32_t    *node_leaf_num_cells(void *node);
void        *node_leaf_cell(void *node, uint32_t cell_num);
uint32_t    *node_leaf_key(void *node, uint32_t cell_num);
void        *node_leaf_value(void *node, uint32_t cell_num);
void        node_leaf_initialize(void *node);
int         node_leaf_insert(cursor_t *cursor, uint32_t key, row_t *value);

#endif

