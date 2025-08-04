#ifndef NODE_H
# define NODE_H

#include <stdint.h>
#include "cursor.h"
#include "row.h"

typedef enum node_type_e
{
    NODE_INTERNAL,
    NODE_LEAF
}   node_type_t;

/**
 * Moves `node` pointer up to num_cells offset.
 *
 * @param node Pointer to node.
 *
 * @return Initial pointer plus num_cells offset. 
 */
uint32_t    *node_leaf_move_to_num_cells(void *node);

/**
 * Moves `node` pointer to `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the cell.
 */
void        *node_leaf_move_to_cell(void *node, uint32_t cell_num);

/**
 * Moves `node` pointer to the key of a given `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the key of the given cell.
 */
uint32_t    *node_leaf_move_to_key(void *node, uint32_t cell_num);

/**
 * Moves `node` pointer to the value of a given `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the value of the given cell.
 */
void        *node_leaf_move_to_value(void *node, uint32_t cell_num);

/**
 * Initializes the given `node`.
 *
 * @param node Pointer to node.
 */
void        node_leaf_init(void *node);

/**
 * Inserts `key`/`value` pair into a node.
 *
 * @param cursor Pointer to cursor.
 * @param key Key that will be inserted in the node.
 * @param value Pointer to value that will be inserted in the node.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         node_leaf_insert(cursor_t *cursor, uint32_t key, row_t *value);

uint32_t    node_leaf_find_cell_num(void *node, uint32_t key);

node_type_t node_get_type(void *node);

void        node_set_type(void *node, node_type_t type);

#endif /* NODE_H */

