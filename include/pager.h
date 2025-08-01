#ifndef PAGER_H
# define PAGER_H

# include <stdint.h>

# define TABLE_MAX_PAGES 100

typedef struct pager_s
{
    int         fd;
    uint32_t    file_length;
    uint32_t    num_pages;
    void        *pages[TABLE_MAX_PAGES];
}   pager_t;

/**
 * Opens new pager.
 *
 * @param filename Path to database.
 *
 * @return Pointer to the new pager or NULL in case of error.
 */
pager_t *pager_open(const char *filename);

/**
 * Gets `page_num` page from `pager`.
 *
 * @param pager Pointer to pager.
 * @param page_num Number of page.
 *
 * @returns Pointer to the requested page or NULL in case of error.
 */
void    *pager_get_page(pager_t *pager, uint32_t page_num);

/**
 * Flush `page_num` page from `pager` into the database.
 *
 * @param pager Pointer to pager.
 * @param page_num Number of page.
 *
 * @return 0 when successful, otherwise returns -1.  
 */
int     pager_flush(pager_t *pager, uint32_t page_num);

#endif /* PAGER_H */

