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

/**
 * Get type of the node.
 *
 * @param node Pointer to node.
 *
 * @return Type of the given node.
 */
node_type_t node_get_type(const void *node);

/**
 * Set type of the node.
 *
 * @param node Pointer to node.
 */
void        node_set_type(void *node, node_type_t type);

/**
 * Create root node.
 *
 * @param table Pointer to table.
 * @param right_child_page_num Number of right child node.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         node_create_root(table_t *table, uint32_t right_child_page_num);

/**
 * Get max key inside the node.
 *
 * @param pager Pointer to pager.
 * @param node Pointer to node.
 *
 * @return The max key found in the node.
 */
uint32_t    node_get_max_key(pager_t *pager, void *node);

/**
 * Check if node is root.
 *
 * @param node Pointer to node.
 *
 * @return True for any number other than 0, which means false.
 */
uint8_t     node_is_root(void *node);

/**
 * Set node's root value.
 *
 * @param node Pointer to node.
 * @param is_root Boolean number to set root value.
 */
void        node_set_root(void *node, uint8_t is_root);

/**
 * Initialize the given `node`.
 *
 * @param node Pointer to node.
 */
void        node_leaf_init(void *node);

/**
 * Insert `key`/`value` pair into a node.
 *
 * @param cursor Pointer to cursor.
 * @param key Key that will be inserted in the node.
 * @param value Pointer to value that will be inserted in the node.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         node_leaf_insert(cursor_t *cursor, const uint32_t key, row_t *value);

/**
 * Given a key, find cell position inside leaf node `page_num`.
 *
 * @param cursor Pointer to cursor.
 * @param pointer Pointer to table.
 * @param page_num Number of page.
 * @param key Value used to find position
 * 
 * @return 0 when successful, otherwise returns -1.
 */
int         node_leaf_find(cursor_t *cursor, table_t *table,
                            const uint32_t page_num, const uint32_t key);

/**
 * Move leaf node pointer up to num_cells offset.
 *
 * @param node Pointer to node.
 *
 * @return Initial pointer plus num_cells offset. 
 */
uint32_t    *node_leaf_move_to_num_cells(void *node);

/**
 * Move leaf node pointer up to next_leaf offset.
 *
 * @param Pointer to node.
 *
 * @return Initial pointe plus next_leaf offset.
 */
uint32_t    *node_leaf_move_to_next_leaf(void *node);

/**
 * Move leaf node pointer to `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the cell.
 */
void        *node_leaf_move_to_cell(void *node, const uint32_t cell_num);

/**
 * Move leaf node pointer to the key of a given `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the key of the given cell.
 */
uint32_t    *node_leaf_move_to_key(void *node, const uint32_t cell_num);

/**
 * Move leaf node pointer to the value of a given `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the value of the given cell.
 */
void        *node_leaf_move_to_value(void *node, const uint32_t cell_num);

/**
 * Initialize internal node.
 *
 * @param node Pointer to node.
 */
void        node_internal_init(void *node);

/**
 * Given a key, find key position inside `page_num`.
 *
 * @param cursor Pointer to cursor.
 * @param table Pointer to table.
 * @param page_num Number of page.
 * @param Number of key.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         node_internal_find(cursor_t *cursor, table_t *table,
                                const uint32_t page_num, const uint32_t key);

/**
 * Move internal node pointer up to num_keys offset.
 *
 * @param node Pointer to node.
 *
 * @return Initial pointer plus num_keys offset.
 */
uint32_t    *node_internal_move_to_num_keys(void *node);

/**
 * Move internal node pointer up to right_child offset.
 *
 * @param node Pointer to node.
 *
 * @return Initial pointer plus right_child offset.
 */
uint32_t    *node_internal_move_to_right_child(void *node);

/**
 * Move internal node pointer to the value of a given `cell_num` cell.
 *
 * @param node Pointer to node.
 * @param cell_num Number of cell.
 *
 * @return Pointer to the value of the given cell.
 */
void        *node_internal_move_to_cell(void *node, uint32_t cell_num);

/**
 * Move internal node pointer to the value of a given `child_num` child.
 *
 * @param node Pointer to node.
 * @param child_num Number of child.
 *
 * @return Pointer to the value of the given child.
 */
uint32_t    *node_internal_move_to_child(void *node, uint32_t child_num);

/**
 * Move internal node pointer to the value of a given `key_num` key.
 *
 * @param node Pointer to node.
 * @param key_num Number of key.
 *
 * @return Pointer to the value of the given key.
 */
uint32_t    *node_internal_move_to_key(void *node, uint32_t key_num);

#endif /* NODE_H */

