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

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H

#include <sharemind/static_assert.h>
#include <stdint.h>
#include "sharemind_executable_read_error.h"
#include "sharemind_executable_section_type.h"


#define SHAREMIND_EXECUTABLE_VERSION_SUPPORTED 0x0u

typedef struct {
    char magic[32];
    uint64_t byteOrderVerification;
    uint16_t fileFormatVersion;
    uint8_t zeroPadding[6];
} SharemindExecutableCommonHeader;
SHAREMIND_STATIC_ASSERT(sizeof(SharemindExecutableCommonHeader) == 32 + 8 + 2 + 6);

void SharemindExecutableCommonHeader_init(SharemindExecutableCommonHeader * h, uint16_t version);
SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableCommonHeader_read(const void * from, SharemindExecutableCommonHeader * h)  __attribute__ ((nonnull(1), warn_unused_result));

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H */
