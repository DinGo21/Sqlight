#ifndef PAGER_H
# define PAGER_H

# include <stdint.h>

# define TABLE_MAX_PAGES 100

typedef struct pager_s
{
    int         fd;
    uint32_t    file_length;
    void        *pages[TABLE_MAX_PAGES];
}   pager_t;

pager_t *pager_open(const char *filename);
void    *pager_get_page(pager_t *pager, uint32_t page_num);
int     pager_flush(pager_t *pager, uint32_t page_num, uint32_t size);

#endif

