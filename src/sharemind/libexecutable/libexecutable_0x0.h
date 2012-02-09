/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBSME_LIBSME_0x0_H
#define SHAREMIND_LIBSME_LIBSME_0x0_H

#include <sharemind/static_assert.h>
#include <stdint.h>
#include "sme_read_error.h"
#include "sme_section_type.h"


typedef struct {
    uint8_t numberOfUnitsMinusOne;
    uint8_t activeLinkingUnit;
    uint8_t zeroPadding[4];
} SME_Header_0x0;
SM_STATIC_ASSERT(sizeof(SME_Header_0x0) == 1u + 1u + 4u);

void SME_Header_0x0_init(SME_Header_0x0 * header,
                         uint8_t numberOfUnitsMinusOne,
                         uint8_t activeLinkingUnit);
SME_Read_Error SME_Header_0x0_read(const void * from, const SME_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));


typedef struct {
    char type[32];
    uint8_t sectionsMinusOne;
    uint8_t zeroPadding[7];
} SME_Unit_Header_0x0;
SM_STATIC_ASSERT(sizeof(SME_Unit_Header_0x0) == 32u + 1u + 7u);

void SME_Unit_Header_0x0_init(SME_Unit_Header_0x0 * header,
                              uint8_t sectionsMinusOne);
SME_Read_Error SME_Unit_Header_0x0_read(const void * from, const SME_Unit_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));

typedef struct {
    char type[32];
    uint32_t length __attribute__ ((packed));
    uint8_t zeroPadding[4];
} SME_Section_Header_0x0;
SM_STATIC_ASSERT(sizeof(SME_Section_Header_0x0) == 32u + 4u + 4u);

void SME_Section_Header_0x0_init(SME_Section_Header_0x0 * header,
                                 SME_Section_Type type,
                                 uint32_t length);
SME_Read_Error SME_Section_Header_0x0_read(const void * from, const SME_Section_Header_0x0 ** h)  __attribute__ ((nonnull(1), warn_unused_result));
SME_Section_Type SME_Section_Header_0x0_type(const SME_Section_Header_0x0 * h) __attribute__ ((nonnull(1), warn_unused_result));

#endif /* SHAREMIND_LIBSME_LIBSME_0x0_H */
