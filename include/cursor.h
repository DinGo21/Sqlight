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
 * Initializes a cursor at the start of `table`.
 *
 * @param table Pointer to the table;
 *
 * @return Pointer to the cursor.
 */
cursor_t    *cursor_init_to_start(table_t *table);

/**
 * Initializes a cursor at the end of `table`.
 *
 * @param table Pointer to the table.
 *
 * @return Pointer to table.
 */
cursor_t    *cursor_init_to_end(table_t *table);

/**
 * Gets the current value `cursor` is pointing to.
 *
 * @param cursor Pointer to the cursor.
 *
 * @return Pointer to the value.
 */
void        *cursor_get_value(cursor_t *cursor);

/**
 * Advances up to one cell from the table `cursor` is pointing to.
 *
 * @param cursor Pointer to the cursor.
 *
 * @return 0 when successful, otherwise returns -1.
 */
int         cursor_advance(cursor_t *cursor);

#endif /* CURSOR_H */

