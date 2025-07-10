#include "sqliteclone.h"

input_buffer_t *
input_buffer_new()
{
    input_buffer_t  *input_buffer;

    input_buffer = malloc(sizeof(*input_buffer));
    if (!input_buffer)
        return NULL;
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;
    return input_buffer;
}

void
input_buffer_close(input_buffer_t *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

