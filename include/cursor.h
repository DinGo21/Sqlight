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

cursor_t    *cursor_table_start(table_t *table);
cursor_t    *cursor_table_end(table_t *table);
void        *cursor_value(cursor_t *cursor);
int         cursor_advance(cursor_t *cursor);

#endif

