#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meta_command.h"
#include "globals.h"
#include "node.h"
#include "pager.h"

static void
meta_command_print_leaf_node(table_t *table)
{
    void        *node;
    uint32_t    i;
    uint32_t    key;
    uint32_t    num_cells;

    node = pager_get_page(table->pager, 0);
    if (node == NULL)
        return;
    num_cells = *node_leaf_move_to_num_cells(node);
    printf("leaf (size %d)\n", num_cells);
    i = 0;
    while (i < num_cells)
    {
        key = *node_leaf_move_to_key(node, i);
        printf(" - %d: %d\n", i, key);
        i++;
    }
}

static void
meta_command_print_constants()
{
    printf("constants:\n");
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

static meta_command_result_t
meta_command_exec(input_buffer_t *input_buffer, table_t *table)
{
    if (!strcmp(input_buffer->buffer, ".exit"))
    {
        input_buffer_close(input_buffer);
        table_free(table);
        exit(EXIT_SUCCESS);
    }
    if (!strcmp(input_buffer->buffer, ".constants"))
    {
        meta_command_print_constants();
        return META_COMMAND_SUCCESS;
    }
    if (!strcmp(input_buffer->buffer, ".btree"))
    {
        meta_command_print_leaf_node(table);
        return META_COMMAND_SUCCESS;
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

void
meta_command_init(input_buffer_t *input_buffer, table_t *table)
{
    switch (meta_command_exec(input_buffer, table))
    {
        case (META_COMMAND_SUCCESS):
            break;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
            printf("Unknown command: %s\n", input_buffer->buffer);
            break;
    }
}

