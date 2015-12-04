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

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H

#include <sharemind/extern_c.h>
#include <sharemind/static_assert.h>
#include <stdint.h>
#include "sharemind_executable_read_error.h"
#include "sharemind_executable_section_type.h"


SHAREMIND_EXTERN_C_BEGIN

/*******************************************************************************
  Format 0x0 header.
*******************************************************************************/

typedef struct {
    uint8_t numberOfUnitsMinusOne;
    uint8_t activeLinkingUnit;
    uint8_t zeroPadding[6];
} SharemindExecutableHeader0x0;
SHAREMIND_STATIC_ASSERT(sizeof(SharemindExecutableHeader0x0) == 1u + 1u + 6u);

void SharemindExecutableHeader0x0_init(
        SharemindExecutableHeader0x0 * header,
        uint8_t numberOfUnitsMinusOne,
        uint8_t activeLinkingUnit);

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableHeader0x0_read(
        const void * from,
        SharemindExecutableHeader0x0 * h)
    __attribute__ ((nonnull(1), warn_unused_result));


/*******************************************************************************
  Format 0x0 unit header.
*******************************************************************************/

typedef struct {
    char type[32];
    uint8_t sectionsMinusOne;
    uint8_t zeroPadding[7];
} SharemindExecutableUnitHeader0x0;
SHAREMIND_STATIC_ASSERT(sizeof(SharemindExecutableUnitHeader0x0) == 32u + 1u + 7u);

void SharemindExecutableUnitHeader0x0_init(
        SharemindExecutableUnitHeader0x0 * header,
        uint8_t sectionsMinusOne);

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableUnitHeader0x0_read(
        const void * from,
        SharemindExecutableUnitHeader0x0 * h)
    __attribute__ ((nonnull(1), warn_unused_result));


/*******************************************************************************
  Format 0x0 section header.
*******************************************************************************/

typedef struct {
    char type[32];
    uint32_t length;
    uint8_t zeroPadding[4];
} SharemindExecutableSectionHeader0x0;
SHAREMIND_STATIC_ASSERT(sizeof(SharemindExecutableSectionHeader0x0) == 32u + 4u + 4u);

void SharemindExecutableSectionHeader0x0_init(
        SharemindExecutableSectionHeader0x0 * header,
        SHAREMIND_EXECUTABLE_SECTION_TYPE type,
        uint32_t length);

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableSectionHeader0x0_read(
        const void * from,
        SharemindExecutableSectionHeader0x0 * h)
    __attribute__ ((nonnull(1), warn_unused_result));

SHAREMIND_EXECUTABLE_SECTION_TYPE SharemindExecutableSectionHeader0x0_type(
        const SharemindExecutableSectionHeader0x0 * h)
    __attribute__ ((nonnull(1), warn_unused_result));

SHAREMIND_EXTERN_C_END

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H */
