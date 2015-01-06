/*
 * Copyright (C) 2015 Cybernetica
 *
 * Research/Commercial License Usage
 * Licensees holding a valid Research License or Commercial License
 * for the Software may use this file according to the written
 * agreement between you and Cybernetica.
 *
 * GNU General Public License Usage
 * Alternatively, this file may be used under the terms of the GNU
 * General Public License version 3.0 as published by the Free Software
 * Foundation and appearing in the file LICENSE.GPL included in the
 * packaging of this file.  Please review the following information to
 * ensure the GNU General Public License version 3.0 requirements will be
 * met: http://www.gnu.org/copyleft/gpl-3.0.html.
 *
 * For further information, please contact us at sharemind@cyber.ee.
 */

#include "libexecutable_0x0.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "endianmacros.h"

static const uint8_t zeroPadding[7u] = { 0u, 0u, 0u, 0u, 0u, 0u, 0u };

void SharemindExecutableHeader0x0_init(SharemindExecutableHeader0x0 * header,
                                       uint8_t numberOfUnitsMinusOne,
                                       uint8_t activeLinkingUnit)
{
    header->numberOfUnitsMinusOne = numberOfUnitsMinusOne;
    header->activeLinkingUnit = activeLinkingUnit;
    __builtin_bzero(header->zeroPadding, 6u);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableHeader0x0_read(
        const void * from,
        SharemindExecutableHeader0x0 * to)
{
    assert(from);

    SharemindExecutableHeader0x0 buf;
    __builtin_memcpy(&buf, from, sizeof(SharemindExecutableHeader0x0));

    if (buf.activeLinkingUnit > buf.numberOfUnitsMinusOne
        || __builtin_memcmp(zeroPadding, buf.zeroPadding, 6u) != 0)
    {
        return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
    }

    if (to)
        __builtin_memcpy(to, &buf, sizeof(SharemindExecutableHeader0x0));

    return SHAREMIND_EXECUTABLE_READ_OK;
}

static const char luMagic[32u] = "Linking Unit";

void SharemindExecutableUnitHeader0x0_init(
        SharemindExecutableUnitHeader0x0 * header,
        uint8_t sectionsMinusOne)
{
    __builtin_memcpy(&header->type, luMagic, 32u);
    header->sectionsMinusOne = sectionsMinusOne;
    __builtin_bzero(&header->zeroPadding, 7u);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableUnitHeader0x0_read(
        const void * from,
        SharemindExecutableUnitHeader0x0 * to)
{
    assert(from);

    SharemindExecutableUnitHeader0x0 buf;
    __builtin_memcpy(&buf, from, sizeof(SharemindExecutableUnitHeader0x0));

    if (__builtin_memcmp(buf.type, luMagic, 32u) != 0
        || buf.sectionsMinusOne > SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0
        || __builtin_memcmp(buf.zeroPadding, zeroPadding, 7u) != 0)
    {
        return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
    }

    if (to)
        __builtin_memcpy(to, &buf, sizeof(SharemindExecutableUnitHeader0x0));

    return SHAREMIND_EXECUTABLE_READ_OK;
}

static const char sMagic[SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0][32u] = {
    "TEXT", "RODATA", "DATA", "BSS", "BIND", "PDBIND", "DEBUG"
};

void SharemindExecutableSectionHeader0x0_init(
        SharemindExecutableSectionHeader0x0 * header,
        SHAREMIND_EXECUTABLE_SECTION_TYPE type,
        uint32_t length)
{
    assert(type < SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0);

    __builtin_memcpy(header->type, sMagic[type], 32u);
    header->length = htole32(length);
    __builtin_bzero(header->zeroPadding, 4u);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableSectionHeader0x0_read(
        const void * from,
        SharemindExecutableSectionHeader0x0 * to)
{
    assert(from);

    SharemindExecutableSectionHeader0x0 buf;
    __builtin_memcpy(&buf, from, sizeof(SharemindExecutableSectionHeader0x0));
    buf.length = le32toh(buf.length);

    for (unsigned i = 0u; i < SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0; i++)
        if (__builtin_memcmp(buf.type, sMagic[i], 32u) == 0)
            goto sharemind_executable_section_header_0x0_read_type_ok;

    goto sharemind_executable_section_header_0x0_read_error;

sharemind_executable_section_header_0x0_read_type_ok:

    if (buf.length <= 0u
        || __builtin_memcmp(buf.zeroPadding, zeroPadding, 4u) != 0)
    {
        goto sharemind_executable_section_header_0x0_read_error;
    }

    if (to)
        __builtin_memcpy(to, &buf, sizeof(SharemindExecutableSectionHeader0x0));

    return SHAREMIND_EXECUTABLE_READ_OK;

sharemind_executable_section_header_0x0_read_error:

    return SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
}

SHAREMIND_EXECUTABLE_SECTION_TYPE SharemindExecutableSectionHeader0x0_type(
        const SharemindExecutableSectionHeader0x0 * h)
{
    assert(h);

#define MATCH_TYPE(e) \
    if ((e) != SHAREMIND_EXECUTABLE_SECTION_TYPE_INVALID \
        && __builtin_memcmp(h->type, sMagic[(e)], 32u) == 0) \
    { \
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
