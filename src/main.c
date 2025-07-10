#include "sqliteclone.h"

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
main()
{
    input_buffer_t  *input_buffer;
    table_t         *table;

    input_buffer = input_buffer_new();
    if (!input_buffer)
        return 1;
    table = table_new();
    if (!table)
    {
        input_buffer_close(input_buffer);
        return 1;
    }
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

