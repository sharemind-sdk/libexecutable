#ifndef LIBSME_LIBSME_H
#define LIBSME_LIBSME_H

#include <stdint.h>
#include "../static_assert.h"


enum SME_Section_Type {
    SME_SECTION_TYPE_TEXT = 0,
    SME_SECTION_TYPE_RODATA = 1,
    SME_SECTION_TYPE_DATA = 2,
    SME_SECTION_TYPE_BSS = 3,
    SME_SECTION_TYPE_BIND = 4,
    SME_SECTION_TYPE_DEBUG = 5,
    SME_SECTION_TYPE_COUNT_0x0 = 6,
    SME_SECTION_TYPE_COUNT = 6
};


struct SME_Common_Header {
    char magic[32];
    uint64_t byte_order_verification __attribute__ ((packed));
    uint16_t file_format_version __attribute__ ((packed));
};
SM_STATIC_ASSERT(sizeof(struct SME_Common_Header) == 32 + 8 + 2);

void SME_Common_Header_init(struct SME_Common_Header * h, uint16_t version);
int SME_Common_Header_read(void * from, struct SME_Common_Header * to);

#endif /* LIBSME_LIBSME_H */
