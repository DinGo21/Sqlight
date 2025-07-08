#include "sqliteclone.h"

static execute_result_t
statement_exec_insert(statement_t *statement, table_t *table)
{
    row_t   *row_to_insert;

    if (table->num_rows >= TABLE_MAX_ROWS)
        return EXECUTE_TABLE_FULL;
    row_to_insert = &statement->row_to_insert;
    row_serialize(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;
    return EXECUTE_SUCCESS;
}

static execute_result_t
statement_exec_select(statement_t *statement, table_t *table)
{
    uint32_t    i;
    row_t       row;

    i = 0;
    (void)statement;
    while (i < table->num_rows)
    {
        row_deserialize(row_slot(table, i), &row);
        printf("(%d, %s, %s)\n", row.id, row.username, row.email);
        i++;
    }
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
statement_exec(statement_t *statement, table_t *table)
{
    switch (statement_exec_type(statement, table))
    {
        case (EXECUTE_SUCCESS):
            printf("Executed\n");
            break;
        case (EXECUTE_TABLE_FULL):
            printf("Error: Table is full\n");
            break;
    }
}

static prepare_result_t
statement_prepare(input_buffer_t *input_buffer, statement_t *statement)
{
    int args;

    if (!strncmp(input_buffer->buffer, "insert", 6))
    {
        statement->type = STATEMENT_INSERT;
        args = sscanf(input_buffer->buffer, "insert %d %s %s", 
                        &statement->row_to_insert.id,
                        statement->row_to_insert.username,
                        statement->row_to_insert.email);
        if (args < 3)
            return PREPARE_SYNTAX_ERROR;
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
statement_init(input_buffer_t *input_buffer, table_t *table)
{
    statement_t statement;

    switch (statement_prepare(input_buffer, &statement))
    {
        case (PREPARE_SUCCESS):
            statement_exec(&statement, table);
            break;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unknown keyword: %s\n", input_buffer->buffer);
            break;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error: %s\n", input_buffer->buffer);
            break;
    }
}

