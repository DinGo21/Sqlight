#ifndef PAGER_H
# define PAGER_H

# include <stdint.h>

# define TABLE_MAX_PAGES 100
# define INVALID_PAGE_NUM UINT32_MAX

typedef struct pager_s
{
    int         fd;
    uint32_t    file_length;
    uint32_t    num_pages;
    void        *pages[TABLE_MAX_PAGES];
}   pager_t;

/**
 * Open new pager.
 *
 * @param filename Path to database.
 *
 * @return Pointer to the new pager or NULL in case of error.
 */
pager_t     *pager_open(const char *filename);

/**
 * Get `page_num` page from `pager`.
 *
 * @param pager Pointer to pager.
 * @param page_num Number of page.
 *
 * @returns Pointer to the requested page or NULL in case of error.
 */
void        *pager_get_page(pager_t *pager, const uint32_t page_num);

//TODO: Add comment
uint32_t    pager_get_unused_page_num(pager_t *pager); 

/**
 * Flush `page_num` page from `pager` into the database.
 *
 * @param pager Pointer to pager.
 * @param page_num Number of page.
 *
 * @return 0 when successful, otherwise returns -1.  
 */
int         pager_flush(pager_t *pager, const uint32_t page_num);

#endif /* PAGER_H */

