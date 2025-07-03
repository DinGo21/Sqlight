#ifndef __SQLITECLONE_H__
# define __SQLITECLONE_H__

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <stdbool.h>

typedef enum meta_command_result_e
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}   meta_command_result_t;

typedef enum prepare_result_e
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT
}   prepare_result_t;

typedef enum statement_type_e
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
}   statement_type_t;

typedef struct input_buffer_s
{
    char    *buffer;
    size_t  buffer_length;
    ssize_t input_length;
}   input_buffer_t;

typedef struct statement_s
{
    statement_type_t    type;
}   statement_t;

#endif /* __SQLITECLONE_H__ */

