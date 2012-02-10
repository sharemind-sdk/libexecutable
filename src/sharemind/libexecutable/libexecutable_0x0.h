/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H

#include <sharemind/static_assert.h>
#include <stdint.h>
#include "sharemind_executable_read_error.h"
#include "sharemind_executable_section_type.h"


/*******************************************************************************
  Format 0x0 header.
*******************************************************************************/

typedef struct __attribute__ ((__packed__)) {
    uint8_t numberOfUnitsMinusOne;
    uint8_t activeLinkingUnit;
    uint8_t zeroPadding[4];
} Sharemind_Executable_Header_0x0;
SHAREMIND_STATIC_ASSERT(sizeof(Sharemind_Executable_Header_0x0) == 1u + 1u + 4u);

void sharemind_executable_header_0x0_init(
        Sharemind_Executable_Header_0x0 * header,
        uint8_t numberOfUnitsMinusOne,
        uint8_t activeLinkingUnit);

SHAREMIND_EXECUTABLE_READ_ERROR sharemind_executable_header_0x0_read(
        const void * from,
        const Sharemind_Executable_Header_0x0 ** h)
    __attribute__ ((nonnull(1), warn_unused_result));


/*******************************************************************************
  Format 0x0 unit header.
*******************************************************************************/

typedef struct __attribute__ ((__packed__)) {
    char type[32];
    uint8_t sectionsMinusOne;
    uint8_t zeroPadding[7];
} Sharemind_Executable_Unit_Header_0x0;
SHAREMIND_STATIC_ASSERT(sizeof(Sharemind_Executable_Unit_Header_0x0) == 32u + 1u + 7u);

void sharemind_executable_unit_header_0x0_init(
        Sharemind_Executable_Unit_Header_0x0 * header,
        uint8_t sectionsMinusOne);

SHAREMIND_EXECUTABLE_READ_ERROR sharemind_executable_unit_header_0x0_read(
        const void * from,
        const Sharemind_Executable_Unit_Header_0x0 ** h)
    __attribute__ ((nonnull(1), warn_unused_result));


/*******************************************************************************
  Format 0x0 section header.
*******************************************************************************/

typedef struct __attribute__ ((__packed__)) {
    char type[32];
    uint32_t length;
    uint8_t zeroPadding[4];
} Sharemind_Executable_Section_Header_0x0;
SHAREMIND_STATIC_ASSERT(sizeof(Sharemind_Executable_Section_Header_0x0) == 32u + 4u + 4u);

void sharemind_executable_section_header_0x0_init(
        Sharemind_Executable_Section_Header_0x0 * header,
        SHAREMIND_EXECUTABLE_SECTION_TYPE type,
        uint32_t length);

SHAREMIND_EXECUTABLE_READ_ERROR sharemind_executable_section_header_0x0_read(
        const void * from,
        const Sharemind_Executable_Section_Header_0x0 ** h)
    __attribute__ ((nonnull(1), warn_unused_result));

SHAREMIND_EXECUTABLE_SECTION_TYPE sharemind_executable_section_header_0x0_type(
        const Sharemind_Executable_Section_Header_0x0 * h)
    __attribute__ ((nonnull(1), warn_unused_result));

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H */
