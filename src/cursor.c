#include <stdlib.h>
#include "cursor.h"
#include "globals.h"

cursor_t *
cursor_table_start(table_t *table)
{
    cursor_t    *cursor;

    cursor = malloc(sizeof(*cursor));
    if (!cursor)
        return NULL;
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);
    return cursor;
}

cursor_t *
cursor_table_end(table_t *table)
{
    cursor_t    *cursor;

    cursor = malloc(sizeof(*cursor));
    if (!cursor)
        return NULL;
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = 1;
    return cursor;
}

void *
cursor_value(cursor_t *cursor)
{
    uint32_t    row_offset;
    uint32_t    byte_offset;
    uint32_t    page_num;
    void        *page;

    page_num = cursor->row_num / ROWS_PER_PAGE;
    page = pager_get_page(cursor->table->pager, page_num);
    if (!page)
        return NULL;
    row_offset = cursor->row_num % ROWS_PER_PAGE;
    byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void
cursor_advance(cursor_t *cursor)
{
    cursor->row_num++;
    if (cursor->row_num >= cursor->table->num_rows)
        cursor->end_of_table = 1;
}
