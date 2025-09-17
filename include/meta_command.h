#ifndef META_COMMAND_H
# define META_COMMAND_H

# include "input_buffer.h"
# include "table.h"

typedef enum meta_command_result_e
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}   meta_command_result_t;

/**
 * Initialize the meta command process.
 *
 * @param input_buffer Pointer to the input buffer.
 * @param table Pointer to the table.
 */
void    meta_command_init(input_buffer_t *input_buffer, table_t *table);

#endif

