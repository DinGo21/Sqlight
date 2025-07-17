#ifndef TABLE_H
# define TABLE_H

# include "pager.h"

typedef struct table_s
{
    uint32_t    num_rows;
    pager_t     *pager;
}   table_t;

table_t *table_new(const char *filename);
void    table_free(table_t *table);

#endif

