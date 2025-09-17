#ifndef STATEMENT_H
# define STATEMENT_H

# include "row.h"
# include "input_buffer.h"

typedef enum statement_type_e
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
}   statement_type_t;

typedef enum prepare_result_e
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT
}   prepare_result_t;

typedef enum execute_result_e
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_DUPLICATE_KEY,
    EXECUTE_FATAL_ERROR
}   execute_result_t;

typedef struct statement_s
{
    statement_type_t    type;
    row_t               row_to_insert;
}   statement_t;

/**
 * Initialize the statement process.
 *
 * @param input_buffer Pointer to the input buffer.
 * @param table Pointer to the table.
 */
void    statement_init(input_buffer_t *input_buffer, table_t *table);

#endif /* STATEMENT_H */

