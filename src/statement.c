#include <stdlib.h>
#include <string.h>
#include "globals.h"
#include "node.h"
#include "statement.h"

static execute_result_t
statement_exec_insert(statement_t *statement, table_t *table)
{
    void        *node;
    cursor_t    *cursor;

    cursor = cursor_table_end(table);
    if (cursor == NULL)
        return EXECUTE_FATAL_ERROR;
    node = pager_get_page(table->pager, table->root_page_num);
    if (node == NULL)
        return EXECUTE_FATAL_ERROR;
    if (*node_leaf_num_cells(node) >= LEAF_NODE_MAX_CELLS)
        return EXECUTE_TABLE_FULL;
    if (node_leaf_insert(cursor, statement->row_to_insert.id,
                &statement->row_to_insert) < 0)
        return EXECUTE_FATAL_ERROR;
    free(cursor);
    return EXECUTE_SUCCESS;
}

static execute_result_t
statement_exec_select(statement_t *statement, table_t *table)
{
    row_t       row;
    void        *slot;
    cursor_t    *cursor;

    (void)statement;
    cursor = cursor_table_start(table);
    if (cursor == NULL)
        return EXECUTE_FATAL_ERROR;
    while (!cursor->end_of_table)
    {
        slot = cursor_value(cursor);
        if (!slot)
            return EXECUTE_FATAL_ERROR;
        row_deserialize(slot, &row);
        printf("(%d, %s, %s)\n", row.id, row.username, row.email);
        if (cursor_advance(cursor) < 0)
            return EXECUTE_FATAL_ERROR;
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

static execute_result_t
statement_exec_type(statement_t *statement, table_t *table)
{
    switch (statement->type)
    {
        case (STATEMENT_INSERT):
            return statement_exec_insert(statement, table);
        case (STATEMENT_SELECT):
            return statement_exec_select(statement, table);
    }
}

static void
statement_exec(statement_t *statement, input_buffer_t *input_buffer,
                table_t *table)
{
    switch (statement_exec_type(statement, table))
    {
        case (EXECUTE_SUCCESS):
            printf("Executed\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table is full\n");
            break;
        case (EXECUTE_FATAL_ERROR):
            printf("Fatal error\n");
            input_buffer_close(input_buffer);
            table_free(table);
            exit(EXIT_FAILURE);
    }
}

static prepare_result_t
statement_prepare_insert(input_buffer_t *input_buffer, statement_t *statement)
{
    int     id;
    char    *id_string;
    char    *username;
    char    *email;

    statement->type = STATEMENT_INSERT;
    strtok(input_buffer->buffer, " ");
    id_string = strtok(NULL, " ");
    username = strtok(NULL, " ");
    email = strtok(NULL, " ");
    if (!id_string || !username || !email)
        return PREPARE_SYNTAX_ERROR;
    id = atoi(id_string);
    if (id < 0)
        return PREPARE_NEGATIVE_ID;
    if (strlen(username) > COLUMN_USERNAME_SIZE)
        return PREPARE_STRING_TOO_LONG;
    if (strlen(email) > COLUMN_EMAIL_SIZE)
        return PREPARE_STRING_TOO_LONG;
    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, username);
    strcpy(statement->row_to_insert.email, email);
    return PREPARE_SUCCESS;
}

static prepare_result_t
statement_prepare(input_buffer_t *input_buffer, statement_t *statement)
{
    if (!strncmp(input_buffer->buffer, "insert", 6))
        return statement_prepare_insert(input_buffer, statement);
    if (!strcmp(input_buffer->buffer, "select"))
        return (statement->type = STATEMENT_SELECT, PREPARE_SUCCESS);
    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void
statement_init(input_buffer_t *input_buffer, table_t *table)
{
    statement_t statement;

    switch (statement_prepare(input_buffer, &statement))
    {
        case (PREPARE_SUCCESS):
            statement_exec(&statement, input_buffer, table);
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unknown keyword: %s\n", input_buffer->buffer);
            break;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error: %s\n", input_buffer->buffer);
            break;
        case (PREPARE_STRING_TOO_LONG):
            printf("Error: string too long\n");
            break;
        case (PREPARE_NEGATIVE_ID):
            printf("Error: negative id\n");
    }
}

