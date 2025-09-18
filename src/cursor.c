#include "cursor.h"
#include "pager.h"
#include "node.h"
#include "table.h"

#include <stdint.h>
#include <stdlib.h>

cursor_t *
cursor_init_to_start(table_t *table)
{
    cursor_t    *cursor;
    void        *node;
    uint32_t    num_cells;

    cursor = cursor_find(table, 0);
    if (cursor == NULL)
        return NULL;
    node = pager_get_page(table->pager, cursor->page_num);
    if (node == NULL)
        return NULL;
    num_cells = *node_leaf_move_to_num_cells(node);
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

cursor_t *
cursor_find(table_t *table, const uint32_t key)
{
    void        *node;
    cursor_t    *cursor;
    int         result;

    node = pager_get_page(table->pager, table->root_page_num);
    if (node == NULL)
        return NULL;
    cursor = malloc(sizeof(*cursor));
    if (cursor == NULL)
        return NULL;
    cursor->table = table;
    cursor->page_num = table->root_page_num;
    cursor->end_of_table = 0;
    switch (node_get_type(node))
    {
        case NODE_LEAF:
            result = node_leaf_find(cursor, table, table->root_page_num, key);
            break;
        case NODE_INTERNAL:
            result = node_internal_find(cursor, table, table->root_page_num,
                                        key);
            break;
    }
    if (result < 0)
        return NULL;
    return cursor;
}

void *
cursor_get_value(const cursor_t *cursor)
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
    void        *node;
    uint32_t    next_page_num;

    node = pager_get_page(cursor->table->pager, cursor->page_num);
    if (node == NULL)
        return -1;
    cursor->cell_num++;
    if (cursor->cell_num < *node_leaf_move_to_num_cells(node))
        return 0;
    next_page_num = *node_leaf_move_to_next_leaf(node);
    if (next_page_num == 0)
        cursor->end_of_table = 1;
    else
    {
        cursor->cell_num = 0;
        cursor->page_num = next_page_num;
    }
    return 0;
}

