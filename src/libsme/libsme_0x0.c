/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "libsme_0x0.h"

#include <assert.h>
#include <stddef.h>

void SME_Header_0x0_init(struct SME_Header_0x0 * header,
                         uint8_t number_of_units_minus_one,
                         uint8_t active_linking_unit)
{
    header->number_of_units_minus_one = number_of_units_minus_one;
    header->active_linking_unit = active_linking_unit;
    __builtin_bzero(header->zeroPadding, 4);
}

enum SME_Read_Error SME_Header_0x0_read(const void * from, const struct SME_Header_0x0 ** h) {
    assert(from);

    union {
        const void * v;
        const struct SME_Header_0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[4] = { 0u, 0u, 0u, 0u };
    if (c.h->active_linking_unit > c.h->number_of_units_minus_one
        || __builtin_memcmp(zeroPadding, c.h->zeroPadding, 4) != 0)
    {
        if (h)
            *h = NULL;
        return SME_READ_ERROR_INVALID_DATA;
    }

    if (h)
        (*h) = c.h;
    return SME_READ_OK;
}

static const char luMagic[32] = "Linking Unit";

void SME_Unit_Header_0x0_init(struct SME_Unit_Header_0x0 * header,
                              uint8_t sections_minus_one)
{
    __builtin_memcpy(&header->type, luMagic, 32);
    header->sections_minus_one = sections_minus_one;
    __builtin_bzero(&header->zeroPadding, 7);
}

enum SME_Read_Error SME_Unit_Header_0x0_read(const void * from, const struct SME_Unit_Header_0x0 ** h) {
    assert(from);
    union {
        const void * v;
        const struct SME_Unit_Header_0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[7] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u };
    if (__builtin_memcmp(c.h->type, luMagic, 32) != 0
        || c.h->sections_minus_one > SME_SECTION_TYPE_COUNT_0x0
        || __builtin_memcmp(c.h->zeroPadding, zeroPadding, 7) != 0)
    {
        if (h)
            (*h) = NULL;
        return SME_READ_ERROR_INVALID_DATA;
    }

    if (h)
        (*h) = c.h;
    return SME_READ_OK;
}

static const char sMagic[SME_SECTION_TYPE_COUNT_0x0][32] = {
    "TEXT", "RODATA", "DATA", "BSS", "BIND", "DEBUG"
};

void SME_Section_Header_0x0_init(struct SME_Section_Header_0x0 * header,
                                 enum SME_Section_Type type,
                                 uint32_t length)
{
    assert(type < SME_SECTION_TYPE_COUNT_0x0);

    __builtin_memcpy(header->type, sMagic[type], 32);
    header->length = length;
    __builtin_bzero(header->zeroPadding, 4);
}

enum SME_Read_Error SME_Section_Header_0x0_read(const void * from, const struct SME_Section_Header_0x0 ** h) {
    assert(from);
    union {
        const void * v;
        const struct SME_Section_Header_0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[4] = { 0u, 0u, 0u, 0u };
    for (unsigned i = 0; i < SME_SECTION_TYPE_COUNT_0x0; i++)
        if (__builtin_memcmp(c.h->type, sMagic[i], 32) == 0)
            goto SME_Section_Header_0x0_read_type_ok;

    goto SME_Section_Header_0x0_read_error;

SME_Section_Header_0x0_read_type_ok:

    if (c.h->length <= 0u || __builtin_memcmp(c.h->zeroPadding, zeroPadding, 4) != 0)
        goto SME_Section_Header_0x0_read_error;

    if (h)
        (*h) = c.h;
    return SME_READ_OK;

SME_Section_Header_0x0_read_error:

    if (h)
        (*h) = NULL;
    return SME_READ_ERROR_INVALID_DATA;
}

enum SME_Section_Type SME_Section_Header_0x0_type(const struct SME_Section_Header_0x0 * h) {
    assert(h);

#define SME_ENUM_Section_Type_match(unused,unused2,e) \
    if (SM_T(2,0,e) != SME_SECTION_TYPE_INVALID && __builtin_memcmp(h->type, sMagic[SM_T(2,0,e)], 32) == 0) \
        return SM_T(2,0,e);

    BOOST_PP_SEQ_FOR_EACH(SME_ENUM_Section_Type_match,_,SME_ENUM_Section_Type)

    return SME_SECTION_TYPE_INVALID;
}
