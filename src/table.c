#include <stdlib.h>
#include <unistd.h>
#include "globals.h"
#include "table.h"

table_t *
table_new(const char *filename)
{
    pager_t     *pager;
    table_t     *table;
    
    pager = pager_open(filename);
    if (!pager)
        return NULL;
    table = malloc(sizeof(*table));
    if (!table)
        return (close(pager->fd), free(pager), NULL);
    table->num_rows = pager->file_length / ROW_SIZE;
    table->pager = pager;
    return table;
}

void
table_free(table_t *table)
{
    uint32_t    i;
    uint32_t    num_full_pages;
    uint32_t    num_additional_rows;
    pager_t     *pager;

    i = 0;
    num_full_pages = table->num_rows / ROWS_PER_PAGE;
    pager = table->pager;
    while (i < num_full_pages)
    {
        while (pager->pages[i] == NULL)
            i++;
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
        i++;
    }
    num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0 && pager->pages[num_full_pages] != NULL)
    {
        pager_flush(pager, num_full_pages, num_additional_rows * ROW_SIZE);
        free(pager->pages[num_full_pages]);
        pager->pages[num_full_pages] = NULL;
    }
    close(pager->fd);
    i = 0;
    while (i < TABLE_MAX_PAGES)
    {
        if (pager->pages[i])
            free(pager->pages[i]);
        i++;
    }
    free(pager);
    free(table);
}

