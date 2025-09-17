#ifndef INPUT_BUFFER_H
# define INPUT_BUFFER_H

# include <unistd.h>
# include <stdio.h>

typedef struct input_buffer_s
{
    char    *buffer;
    size_t  buffer_length;
    ssize_t input_length;
}   input_buffer_t;

/**
 * Create new input buffer.
 *
 * @return Pointer to the input buffer or NULL in case of error.
 */
input_buffer_t  *input_buffer_new();

/**
 * Close `input_buffer`.
 *
 * @param input_buffer Pointer to the input buffer.
 */
void            input_buffer_close(input_buffer_t *input_buffer);

#endif

