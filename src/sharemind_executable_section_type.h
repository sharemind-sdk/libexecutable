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

#ifndef SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_SECTION_TYPE_H
#define SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_SECTION_TYPE_H

#include <sharemind/preprocessor.h>


#define SHAREMIND_EXECUTABLE_ENUM_SECTION_TYPE \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_INVALID, = -1)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_TEXT, = 0)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_RODATA, = 1)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_DATA, = 2)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_BSS, = 3)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_BIND, = 4)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_PDBIND, = 5)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_DEBUG, = 6)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT_0x0, = 7)) \
    ((SHAREMIND_EXECUTABLE_SECTION_TYPE_COUNT, = 7))
SHAREMIND_ENUM_CUSTOM_DEFINE(SHAREMIND_EXECUTABLE_SECTION_TYPE,
                             SHAREMIND_EXECUTABLE_ENUM_SECTION_TYPE);

#endif /* SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_SECTION_TYPE_H */
