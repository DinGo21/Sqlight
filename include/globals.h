#ifndef GLOBALS_H
# define GLOBALS_H

# include <stdint.h>

# define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)

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

extern const uint32_t   NODE_TYPE_SIZE;
extern const uint32_t   NODE_TYPE_OFFSET;
extern const uint32_t   IS_ROOT_SIZE;
extern const uint32_t   IS_ROOT_OFFSET;
extern const uint32_t   PARENT_POINTER_SIZE;
extern const uint32_t   PARENT_POINTER_OFFSET;
extern const uint8_t    COMMON_NODE_HEADER_SIZE;

extern const uint32_t   LEAF_NODE_NUM_CELLS_SIZE;
extern const uint32_t   LEAF_NODE_NUM_CELLS_OFFSET;
extern const uint32_t   LEAF_NODE_HEADER_SIZE;
extern const uint32_t   LEAF_NODE_KEY_SIZE;
extern const uint32_t   LEAF_NODE_KEY_OFFSET;
extern const uint32_t   LEAF_NODE_VALUE_SIZE;
extern const uint32_t   LEAF_NODE_VALUE_OFFSET;
extern const uint32_t   LEAF_NODE_CELL_SIZE;
extern const uint32_t   LEAF_NODE_SPACE_FOR_CELLS;
extern const uint32_t   LEAF_NODE_MAX_CELLS;
extern const uint32_t   LEAF_NODE_RIGHT_SPLIT_COUNT;
extern const uint32_t   LEAF_NODE_LEFT_SPLIT_COUNT;

extern const uint32_t   INTERNAL_NODE_NUM_KEYS_SIZE;
extern const uint32_t   INTERNAL_NODE_NUM_KEYS_OFFSET;
extern const uint32_t   INTERNAL_NODE_RIGHT_CHILD_SIZE;
extern const uint32_t   INTERNAL_NODE_RIGHT_CHILD_OFFSET;
extern const uint32_t   INTERNAL_NODE_HEADER_SIZE;
extern const uint32_t   INTERNAL_NODE_KEY_SIZE;
extern const uint32_t   INTERNAL_NODE_CHILD_SIZE;
extern const uint32_t   INTERNAL_NODE_CELL_SIZE;

#endif

