#include "sqliteclone.h"
#include <stdlib.h>

input_buffer_t *
new_input_buffer()
{
    input_buffer_t  *input_buffer;

    input_buffer = malloc(sizeof(*input_buffer));
    if (!input_buffer)
        exit(EXIT_FAILURE);
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void
close_input_buffer(input_buffer_t *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

void
print_prompt()
{
    printf("db > ");
}

void
read_input(input_buffer_t *input_buffer)
{
    ssize_t bytes_read;

    bytes_read = getline(&input_buffer->buffer, &input_buffer->buffer_length,
                            stdin);
    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        close_input_buffer(input_buffer);
        exit(EXIT_FAILURE);
    }
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

meta_command_result_t
meta_command_exec(input_buffer_t *input_buffer)
{
    if (!strcmp(input_buffer->buffer, ".exit"))
    {
        close_input_buffer(input_buffer);
        exit(EXIT_SUCCESS);
    }
    return META_COMMAND_UNRECOGNIZED_COMMAND;
}

void
meta_command(input_buffer_t *input_buffer)
{
    switch(meta_command_exec(input_buffer))
    {
        case (META_COMMAND_SUCCESS):
            break;
        case (META_COMMAND_UNRECOGNIZED_COMMAND):
            printf("Unknown command: %s\n", input_buffer->buffer);
            break;
    }
}

prepare_result_t
statement_prepare(input_buffer_t *input_buffer, statement_t *statement)
{
    if (!strncmp(input_buffer->buffer, "insert", 6))
    {
        statement->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }
    if (!strcmp(input_buffer->buffer, "select"))
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void
statement_exec(statement_t *statement)
{
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            printf("insert\n");
            break;
        case (STATEMENT_SELECT):
            printf("select\n");
            break;
    }
}

void
statement(input_buffer_t *input_buffer)
{
    statement_t statement;

    switch(statement_prepare(input_buffer, &statement))
    {
        case (PREPARE_SUCCESS):
            statement_exec(&statement);
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unknown keyword: %s\n", input_buffer->buffer);
            return;
            break;
    }
}

int
main(void)
{
    input_buffer_t  *input_buffer;

    input_buffer = new_input_buffer();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);
        if (input_buffer->buffer[0] == '.')
            meta_command(input_buffer);
        else
            statement(input_buffer);
    }
    close_input_buffer(input_buffer);
    return 0;
}

