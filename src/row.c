#include <string.h>
#include "globals.h"
#include "row.h"

void
row_serialize(row_t *src, void *dest)
{
    memcpy(dest + ID_OFFSET, &src->id, ID_SIZE);
    memcpy(dest + USERNAME_OFFSET, &src->username, USERNAME_SIZE);
    memcpy(dest + EMAIL_OFFSET, &src->email, EMAIL_SIZE);
}

void
row_deserialize(void *src, row_t *dest)
{
    memcpy(&dest->id, src + ID_OFFSET, ID_SIZE);
    memcpy(&dest->username, src + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&dest->email, src + EMAIL_OFFSET, EMAIL_SIZE);
}

