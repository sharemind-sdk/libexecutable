#ifndef LIBSME_LIBSME_0x0_H
#define LIBSME_LIBSME_0x0_H

#include "libsme.h"


struct SME_Header_0x0 {
    uint8_t number_of_units_minus_one;
    uint8_t active_linking_unit;
    uint8_t zeroPadding[4];
};
SM_STATIC_ASSERT(sizeof(struct SME_Header_0x0) == 1u + 1u + 4u);

void SME_Header_0x0_init(struct SME_Header_0x0 * header,
                         uint8_t number_of_units_minus_one,
                         uint8_t active_linking_unit);


struct SME_Unit_Header_0x0 {
    char type[32];
    uint8_t sections_minus_one;
    uint8_t zeroPadding[7];
};
SM_STATIC_ASSERT(sizeof(struct SME_Unit_Header_0x0) == 32u + 1u + 7u);

void SME_Unit_Header_0x0_init(struct SME_Unit_Header_0x0 * header,
                              uint8_t sections_minus_one);


struct SME_Section_Header_0x0 {
    char type[32];
    uint32_t length __attribute__ ((packed));
    uint8_t zeroPadding[4];
};
SM_STATIC_ASSERT(sizeof(struct SME_Section_Header_0x0) == 32u + 4u + 4u);

void SME_Section_Header_0x0_init(struct SME_Section_Header_0x0 * header,
                                 enum SME_Section_Type type,
                                 uint32_t length);

#endif /* LIBSME_LIBSME_0x0_H */
