#ifndef CURSOR_H
# define CURSOR_H

#include "table.h"

typedef struct cursor_s
{
    table_t     *table;
    uint32_t    page_num;
    uint32_t    cell_num;
    int         end_of_table;
}   cursor_t;

/**
 * Initialize a cursor at the start of `table`.
 *
 * @param table Pointer to the table;
 *
 * @return Pointer to the cursor.
 */
cursor_t    *cursor_init_to_start(table_t *table);

/**
 * Initialize a cursor at the end of `table`.
 *
 * @param table Pointer to the table.
 *
 * @return Pointer to table.
 */
cursor_t    *cursor_init_to_end(table_t *table);

/**
 * Return the position of the given key.
 *
 * @param table Pointer to the table.
 * @param key Key to find in the table.
 *
 * @return Pointer to the cursor positioned at the key, if the key is not
 * present returns the position where it should be or NULL in case of error.
 */
cursor_t    *cursor_find(table_t *table, const uint32_t key);

/**
 * Get the current value `cursor` is pointing to.
 *
 * @param cursor Pointer to the cursor.
 *
 * @return Pointer to the value.
 */
void        *cursor_get_value(const cursor_t *cursor);

/**
 * Advance up to one cell from the table `cursor` is pointing to.
 *
 * @param cursor Pointer to the cursor.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         cursor_advance(cursor_t *cursor);

#endif /* CURSOR_H */

