#ifndef ROW_H
# define ROW_H

# include "table.h"

# define COLUMN_USERNAME_SIZE 32
# define COLUMN_EMAIL_SIZE 255

typedef struct row_s
{
    uint32_t    id;
    char        username[COLUMN_USERNAME_SIZE + 1];
    char        email[COLUMN_EMAIL_SIZE + 1];
}   row_t;

/**
 * Serializes `src` row into `dest`.
 *
 * @param src Pointer to the row.
 * @param dest Pointer to the destination.
 */
void    row_serialize(row_t *src, void *dest);

/**
 * Deserializes `src` into `dest` row.
 *
 * @param src Pointer to the source.
 * @param dest Pointer to the row.
 */
void    row_deserialize(void *src, row_t *dest);
void    *row_slot(table_t *table, uint32_t row_num);

#endif /* ROW_H */

