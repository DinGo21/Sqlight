#ifndef TABLE_H
# define TABLE_H

# include "pager.h"

typedef struct table_s
{
    uint32_t    root_page_num;
    pager_t     *pager;
}   table_t;

/**
 * Create new table.
 *
 * @param filename Path to database.
 *
 * @return Pointer to the new table or NULL in case of error.
 */
table_t *table_new(const char *filename);

/**
 * Free `table`.
 *
 * @param table Pointer to the table;
 */
void    table_free(table_t *table);

#endif /* TABLE_H */

