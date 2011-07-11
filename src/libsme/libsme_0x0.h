/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef LIBSME_LIBSME_0x0_H
#define LIBSME_LIBSME_0x0_H

#include <stdint.h>
#include "../static_assert.h"
#include "sme_read_error.h"
#include "sme_section_type.h"


struct SME_Header_0x0 {
    uint8_t number_of_units_minus_one;
    uint8_t active_linking_unit;
    uint8_t zeroPadding[4];
};
SM_STATIC_ASSERT(sizeof(struct SME_Header_0x0) == 1u + 1u + 4u);

void SME_Header_0x0_init(struct SME_Header_0x0 * header,
                         uint8_t number_of_units_minus_one,
                         uint8_t active_linking_unit);
enum SME_Read_Error SME_Header_0x0_read(const void * from, const struct SME_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));


struct SME_Unit_Header_0x0 {
    char type[32];
    uint8_t sections_minus_one;
    uint8_t zeroPadding[7];
};
SM_STATIC_ASSERT(sizeof(struct SME_Unit_Header_0x0) == 32u + 1u + 7u);

void SME_Unit_Header_0x0_init(struct SME_Unit_Header_0x0 * header,
                              uint8_t sections_minus_one);
enum SME_Read_Error SME_Unit_Header_0x0_read(const void * from, const struct SME_Unit_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));

struct SME_Section_Header_0x0 {
    char type[32];
    uint32_t length __attribute__ ((packed));
    uint8_t zeroPadding[4];
};
SM_STATIC_ASSERT(sizeof(struct SME_Section_Header_0x0) == 32u + 4u + 4u);

void SME_Section_Header_0x0_init(struct SME_Section_Header_0x0 * header,
                                 enum SME_Section_Type type,
                                 uint32_t length);
enum SME_Read_Error SME_Section_Header_0x0_read(const void * from, const struct SME_Section_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));
enum SME_Section_Type SME_Section_Header_0x0_type(const struct SME_Section_Header_0x0 * h) __attribute__ ((nonnull(1), warn_unused_result));

#endif /* LIBSME_LIBSME_0x0_H */
