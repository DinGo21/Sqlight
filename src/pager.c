#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "globals.h"
#include "pager.h"

int
pager_flush(pager_t *pager, uint32_t page_num)
{
    if (!pager->pages[page_num])
        return -1;
    if (lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET) < 0)
        return -1;
    if (write(pager->fd, pager->pages[page_num], PAGE_SIZE) < 0)
        return -1;
    return 0;
}

void *
pager_get_page(pager_t *pager, uint32_t page_num)
{
    uint32_t    num_pages;
    ssize_t     bytes_read;
    void        *page;

    if (page_num > TABLE_MAX_PAGES)
        return NULL;
    if (pager->pages[page_num])
        return pager->pages[page_num];
    page = malloc(PAGE_SIZE);
    if (page == NULL)
        return NULL;
    num_pages = pager->file_length / PAGE_SIZE;
    if (pager->file_length % PAGE_SIZE)
        num_pages += 1;
    bytes_read = 0;
    if (page_num <= num_pages)
    {
        lseek(pager->fd, page_num * PAGE_SIZE, SEEK_SET);
        bytes_read = read(pager->fd, page, PAGE_SIZE);
    }
    if (bytes_read < 0)
        return (free(page), NULL);
    pager->pages[page_num] = page;
    if (page_num >= pager->num_pages)
        pager->num_pages = page_num + 1;
    return pager->pages[page_num];
}

pager_t *
pager_open(const char *filename)
{
    int     i;
    int     fd;
    pager_t *pager;

    fd = open(filename, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR);
    if (fd < 0)
        return NULL;
    pager = malloc(sizeof(*pager));
    if (pager == NULL)
        return (close(fd), NULL);
    pager->fd = fd;
    pager->file_length = lseek(fd, 0, SEEK_END);
    pager->num_pages = (pager->file_length / PAGE_SIZE);
    if (pager->file_length % PAGE_SIZE != 0)
    {
        printf("DB file is not a whole number of pages. Corrupt file\n");
        return (close(fd), free(pager), NULL);
    }
    i = 0;
    while (i < TABLE_MAX_PAGES)
        pager->pages[i++] = NULL;
    return pager;
}

