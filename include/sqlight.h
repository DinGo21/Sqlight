#ifndef __SQLIGHT_H__
# define __SQLIGHT_H__

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <string.h>
# include <stdbool.h>
# include <stdint.h>

# define TABLE_MAX_PAGES 100
# define COLUMN_USERNAME_SIZE 32
# define COLUMN_EMAIL_SIZE 255
# define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

// globals.c

extern const uint32_t  ID_SIZE;
extern const uint32_t  USERNAME_SIZE;
extern const uint32_t  EMAIL_SIZE;
extern const uint32_t  ID_OFFSET;
extern const uint32_t  USERNAME_OFFSET;
extern const uint32_t  EMAIL_OFFSET;
extern const uint32_t  ROW_SIZE;
extern const uint32_t  PAGE_SIZE;
extern const uint32_t  ROWS_PER_PAGE;
extern const uint32_t  TABLE_MAX_ROWS;


typedef enum meta_command_result_e
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
}   meta_command_result_t;

typedef enum prepare_result_e
{
    PREPARE_SUCCESS,
    PREPARE_SYNTAX_ERROR,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_UNRECOGNIZED_STATEMENT
}   prepare_result_t;

typedef enum statement_type_e
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
}   statement_type_t;

typedef enum execute_result_e
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_FATAL_ERROR
}   execute_result_t;

typedef struct input_buffer_s
{
    char    *buffer;
    size_t  buffer_length;
    ssize_t input_length;
}   input_buffer_t;

typedef struct pager_s
{
    int         fd;
    uint32_t    file_length;
    void        *pages[TABLE_MAX_PAGES];
}   pager_t;

typedef struct table_s
{
    uint32_t    num_rows;
    pager_t     *pager;
}   table_t;

typedef struct row_s
{
    uint32_t    id;
    char        username[COLUMN_USERNAME_SIZE + 1];
    char        email[COLUMN_EMAIL_SIZE + 1];
}   row_t;

typedef struct statement_s
{
    statement_type_t    type;
    row_t               row_to_insert;
}   statement_t;

// input_buffer.c

input_buffer_t  *input_buffer_new();
void            input_buffer_close(input_buffer_t *input_buffer);

// pager.c

pager_t         *pager_open(const char *filename);
void            *pager_get_page(pager_t *pager, uint32_t page_num);
int             pager_flush(pager_t *pager, uint32_t page_num, uint32_t size);

// table.c

table_t         *table_new(const char *filename);
void            table_free(table_t *table);

// row.c

void            row_serialize(row_t *src, void *dest);
void            row_deserialize(void *src, row_t *dest);
void            *row_slot(table_t *table, uint32_t row_num);

// meta_command.c

void            meta_command_init(input_buffer_t *input_buffer, table_t *table);

// statement.c

void            statement_init(input_buffer_t *input_buffer, table_t *table);

#endif /* __SQLITECLONE_H__ */

