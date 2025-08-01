#include "globals.h"
#include "row.h"

const uint32_t  ID_SIZE = size_of_attribute(row_t, id);
const uint32_t  USERNAME_SIZE = size_of_attribute(row_t, username);
const uint32_t  EMAIL_SIZE = size_of_attribute(row_t, email);
const uint32_t  ID_OFFSET = 0;
const uint32_t  USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t  EMAIL_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t  ROW_SIZE = ID_SIZE + USERNAME_SIZE + EMAIL_SIZE;
const uint32_t  PAGE_SIZE = 4096;

const uint32_t  NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t  NODE_TYPE_OFFSET = 0;
const uint32_t  IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t  IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t  PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t  PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t   COMMON_NODE_HEADER_SIZE = NODE_TYPE_SIZE + IS_ROOT_SIZE +
                                            PARENT_POINTER_SIZE;

const uint32_t  LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t  LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t  LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                        LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t  LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t  LEAF_NODE_KEY_OFFSET = 0;
const uint32_t  LEAF_NODE_VALUE_SIZE = ROW_SIZE;
const uint32_t  LEAF_NODE_VALUE_OFFSET = LEAF_NODE_KEY_OFFSET +
                                        LEAF_NODE_KEY_SIZE;
const uint32_t  LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t  LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t  LEAF_NODE_MAX_CELLS = LEAF_NODE_SPACE_FOR_CELLS /
                                        LEAF_NODE_CELL_SIZE;
