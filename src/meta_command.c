#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "meta_command.h"
#include "print.h"

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
        print_constants();
        return META_COMMAND_SUCCESS;
    }
    if (!strcmp(input_buffer->buffer, ".btree"))
    {
        print_tree(table->pager, 0, 0);
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

