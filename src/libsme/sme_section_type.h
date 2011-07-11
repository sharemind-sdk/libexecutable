#ifndef LIBSME_SME_SECTION_TYPE_H
#define LIBSME_SME_SECTION_TYPE_H

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

#endif /* LIBSME_SME_SECTION_TYPE_H */
