#include <stdio.h>
#include <stdbool.h>
#include "input_buffer.h"
#include "meta_command.h"
#include "statement.h"
#include "table.h"

void
print_prompt()
{
    printf("db > ");
}

int
read_input(input_buffer_t *input_buffer)
{
    ssize_t bytes_read;

    bytes_read = getline(&input_buffer->buffer, &input_buffer->buffer_length,
                            stdin);
    if (bytes_read <= 0)
        return -1;
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
    return 0;
}

int
main(int argc, char *argv[])
{
    input_buffer_t  *input_buffer;
    table_t         *table;

    if (argc < 2)
        return (printf("Database filename required\n"), 1);
    input_buffer = input_buffer_new();
    if (!input_buffer)
        return 1;
    table = table_new(argv[1]);
    if (!table)
        return (input_buffer_close(input_buffer), 1);
    while (true)
    {
        print_prompt();
        if (read_input(input_buffer) < 0)
        {
            printf("Error reading input\n");
            input_buffer_close(input_buffer);
            table_free(table);
            return 1;
        }
        if (input_buffer->buffer[0] == '.')
            meta_command_init(input_buffer, table);
        else
            statement_init(input_buffer, table);
    }
}

