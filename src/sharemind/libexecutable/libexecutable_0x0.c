/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "libexecutable_0x0.h"

#include <assert.h>
#include <stddef.h>

void SharemindExecutableHeader0x0_init(SharemindExecutableHeader0x0 * header,
                         uint8_t numberOfUnitsMinusOne,
                         uint8_t activeLinkingUnit)
{
    header->numberOfUnitsMinusOne = numberOfUnitsMinusOne;
    header->activeLinkingUnit = activeLinkingUnit;
    __builtin_bzero(header->zeroPadding, 4);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableHeader0x0_read(const void * from, const SharemindExecutableHeader0x0 ** h) {
    assert(from);

    union {
        const void * v;
        const SharemindExecutableHeader0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[4] = { 0u, 0u, 0u, 0u };
    if (c.h->activeLinkingUnit > c.h->numberOfUnitsMinusOne
        || __builtin_memcmp(zeroPadding, c.h->zeroPadding, 4) != 0)
    {
        if (h)
            *h = NULL;
        return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
    }

    if (h)
        (*h) = c.h;
    return SHAREMIND_EXECUTABLE_READ_OK;
}

static const char luMagic[32] = "Linking Unit";

void SharemindExecutableUnitHeader0x0_init(SharemindExecutableUnitHeader0x0 * header,
                              uint8_t sectionsMinusOne)
{
    __builtin_memcpy(&header->type, luMagic, 32);
    header->sectionsMinusOne = sectionsMinusOne;
    __builtin_bzero(&header->zeroPadding, 7);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableUnitHeader0x0_read(const void * from, const SharemindExecutableUnitHeader0x0 ** h) {
    assert(from);
    union {
        const void * v;
        const SharemindExecutableUnitHeader0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[7] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u };
    if (__builtin_memcmp(c.h->type, luMagic, 32) != 0
        || c.h->sectionsMinusOne > SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0
        || __builtin_memcmp(c.h->zeroPadding, zeroPadding, 7) != 0)
    {
        if (h)
            (*h) = NULL;
        return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
    }

    if (h)
        (*h) = c.h;
    return SHAREMIND_EXECUTABLE_READ_OK;
}

static const char sMagic[SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0][32] = {
    "TEXT", "RODATA", "DATA", "BSS", "BIND", "PDBIND", "DEBUG"
};

void SharemindExecutableSectionHeader0x0_init(SharemindExecutableSectionHeader0x0 * header,
                                 SHAREMIND_EXECUTABLE_SECTION_TYPE type,
                                 uint32_t length)
{
    assert(type < SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0);

    __builtin_memcpy(header->type, sMagic[type], 32);
    header->length = length;
    __builtin_bzero(header->zeroPadding, 4);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableSectionHeader0x0_read(const void * from, const SharemindExecutableSectionHeader0x0 ** h) {
    assert(from);
    union {
        const void * v;
        const SharemindExecutableSectionHeader0x0 * h;
    } c = { .v = from };

    static const uint8_t zeroPadding[4] = { 0u, 0u, 0u, 0u };
    for (unsigned i = 0; i < SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0; i++)
        if (__builtin_memcmp(c.h->type, sMagic[i], 32) == 0)
            goto sharemind_executable_section_header_0x0_read_type_ok;

    goto sharemind_executable_section_header_0x0_read_error;

sharemind_executable_section_header_0x0_read_type_ok:

    if (c.h->length <= 0u || __builtin_memcmp(c.h->zeroPadding, zeroPadding, 4) != 0)
        goto sharemind_executable_section_header_0x0_read_error;

    if (h)
        (*h) = c.h;
    return SHAREMIND_EXECUTABLE_READ_OK;

sharemind_executable_section_header_0x0_read_error:

    if (h)
        (*h) = NULL;
    return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
}

SHAREMIND_EXECUTABLE_SECTION_TYPE SharemindExecutableSectionHeader0x0_type(const SharemindExecutableSectionHeader0x0 * h) {
    assert(h);

#define MATCH_TYPE(e) \
    if ((e) != SHAREMIND_EXECUTABLE_SECTION_TYPE_INVALID && __builtin_memcmp(h->type, sMagic[(e)], 32) == 0) { \
        return (e); \
    } else (void) 0

    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_TEXT);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_RODATA);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_DATA);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_BSS);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_BIND);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_PDBIND);
    MATCH_TYPE(SHAREMIND_EXECUTABLE_SECTION_TYPE_DEBUG);

    return SHAREMIND_EXECUTABLE_SECTION_TYPE_INVALID;
}
