#include "sqliteclone.h"

table_t *
table_new()
{
    uint32_t    i;
    table_t     *table;
    
    table = malloc(sizeof(*table));
    if (!table)
        return NULL;
    i = 0;
    table->num_rows = 0;
    while (i < TABLE_MAX_PAGES)
        table->pages[i++] = NULL;
    return table;
}

void
table_free(table_t *table)
{
    int i;

    i = 0;
    while (table->pages[i])
        free(table->pages[i++]);
    free(table);
}

