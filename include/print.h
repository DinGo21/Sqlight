#ifndef PRINT_H
# define PRINT_H

# include "pager.h"

# include <stdint.h>

void    print_constants();

void    print_tree(pager_t *pager, const uint32_t page_num,
                    const int indent_level);

#endif

