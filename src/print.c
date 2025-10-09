#include "print.h"
#include "globals.h"
#include "node.h"

#include <stdio.h>

void 
print_constants()
{
    printf("constants:\n");
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
    printf("LEAF_NODE_LEFT_SPLIT_COUNT: %d\n", LEAF_NODE_LEFT_SPLIT_COUNT);
}

static void
print_indent(const int level)
{
    for (int i = 0; i < level; i++)
        printf("    ");
}

static void
print_node_leaf(void *node, const int indent_level)
{
    uint32_t    i;
    uint32_t    num_keys;

    num_keys = *node_leaf_move_to_num_cells(node);
    print_indent(indent_level);
    printf("- leaf (size %d)\n", num_keys);
    i = 0;
    while (i < num_keys)
    {
        print_indent(indent_level + 1);
        printf("- %d\n", *node_leaf_move_to_key(node, i));
        i++;
    }
}

static void
print_node_internal(void *pager, void *node, const int indent_level)
{
    uint32_t    num_keys;
    uint32_t    child;


    num_keys = *node_internal_move_to_num_keys(node);
    print_indent(indent_level);
    printf("- internal (size %d)\n", num_keys);
    if (num_keys == 0)
        return;
    for (uint32_t i = 0; i < num_keys; i++)
    {
        child = *node_internal_move_to_child(node, i);
        print_tree(pager, child, indent_level + 1);
        print_indent(indent_level + 1);
        printf("- key %d\n", *node_internal_move_to_key(node, i));
    }
    child = *node_internal_move_to_right_child(node);
    print_tree(pager, child, indent_level + 1);
}

void
print_tree(pager_t *pager, const uint32_t page_num, const int indent_level)
{
    void        *node;

    node = pager_get_page(pager, page_num);
    if (node == NULL)
        return;
    switch (node_get_type(node))
    {
        case NODE_LEAF:
            print_node_leaf(node, indent_level);
            break;
        case NODE_INTERNAL:
            print_node_internal(pager, node, indent_level);
            break;
    }
}
