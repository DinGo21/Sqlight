#include <stdlib.h>
#include "cursor.h"
#include "pager.h"
#include "node.h"

cursor_t *
cursor_init_to_start(table_t *table)
{
    cursor_t    *cursor;
    void        *root_node;
    uint32_t    num_cells;

    cursor = malloc(sizeof(*cursor));
    if (cursor == NULL)
        return NULL;
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->cell_num = 0;
    root_node = pager_get_page(table->pager, table->root_page_num);
    if (root_node == NULL)
        return NULL;
    num_cells = *node_leaf_move_to_num_cells(root_node);
    cursor->end_of_table = (num_cells == 0);
    return cursor;
}

cursor_t *
cursor_init_to_end(table_t *table)
{
    cursor_t    *cursor;
    void        *root_node;

    cursor = malloc(sizeof(*cursor));
    if (cursor == NULL)
        return NULL;
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    root_node = pager_get_page(table->pager, table->root_page_num);
    if (root_node == NULL)
        return NULL;
    cursor->cell_num = *node_leaf_move_to_num_cells(root_node);
    cursor->end_of_table = 1;
    return cursor;
}

void *
cursor_get_value(cursor_t *cursor)
{
    void        *page;

    page = pager_get_page(cursor->table->pager, cursor->page_num);
    if (page == NULL)
        return NULL;
    return node_leaf_move_to_value(page, cursor->cell_num);
}

int
cursor_advance(cursor_t *cursor)
{
    void    *node;

    node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (node == NULL)
        return -1;
    cursor->cell_num++;
    if (cursor->cell_num >= (*node_leaf_move_to_num_cells(node)))
        cursor->end_of_table = 1;
    return 0;
}

