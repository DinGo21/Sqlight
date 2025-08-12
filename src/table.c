#include <stdlib.h>
#include <unistd.h>
#include "node.h"
#include "pager.h"
#include "table.h"

table_t *
table_new(const char *filename)
{
    void        *root_node;
    pager_t     *pager;
    table_t     *table;
    
    pager = pager_open(filename);
    if (pager == NULL)
        return NULL;
    table = malloc(sizeof(*table));
    if (table == NULL)
        return (close(pager->fd), free(pager), NULL);
    table->root_page_num = 0;
    if (!pager->num_pages)
    {
        root_node = pager_get_page(pager, 0);
        node_leaf_init(root_node);
        node_set_root(root_node, 1);
    }
    table->pager = pager;
    return table;
}

void
table_free(table_t *table)
{
    uint32_t    i;
    pager_t     *pager;

    i = 0;
    pager = table->pager;
    while (i < pager->num_pages)
    {
        while (pager->pages[i] == NULL)
            i++;
        pager_flush(pager, i);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
        i++;
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

