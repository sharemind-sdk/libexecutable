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

#include "libexecutable.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "endianmacros.h"


static const char magic[32] = "Sharemind Executable";

void SharemindExecutableCommonHeader_init(SharemindExecutableCommonHeader * h,
                                          uint16_t version)
{
    __builtin_memcpy(h->magic, magic, 32);
    h->byteOrderVerification = htole64(UINT64_C(0x0123456789abcdef));
    h->fileFormatVersion = htole16(version);
    __builtin_bzero(h->zeroPadding, 6);
}

static const uint8_t zeroPadding[6u] = { 0u, 0u, 0u, 0u, 0u, 0u };

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableCommonHeader_read(
        const void * from,
        SharemindExecutableCommonHeader * to)
{
    assert(from);

    SharemindExecutableCommonHeader buf;
    __builtin_memcpy(&buf, from, sizeof(SharemindExecutableCommonHeader));
    buf.byteOrderVerification = le64toh(buf.byteOrderVerification);
    buf.fileFormatVersion = le16toh(buf.fileFormatVersion);

    SHAREMIND_EXECUTABLE_READ_ERROR err;

    if (__builtin_memcmp(buf.magic, magic, 32) != 0) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_MAGIC;
        goto sharemind_executable_common_header_read_error;
    }

    if (buf.byteOrderVerification != UINT64_C(0x0123456789abcdef)) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_BYTE_ORDER_VERIFICATION;
        goto sharemind_executable_common_header_read_error;
    }

    if (buf.fileFormatVersion > SHAREMIND_EXECUTABLE_VERSION_SUPPORTED) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_VERSION_NOT_SUPPORTED;
        goto sharemind_executable_common_header_read_error;
    }

    if (__builtin_memcmp(buf.zeroPadding, zeroPadding, 6u) != 0) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
        goto sharemind_executable_common_header_read_error;
    }

    if (to)
        __builtin_memcpy(to, &buf, sizeof(SharemindExecutableCommonHeader));

    return SHAREMIND_EXECUTABLE_READ_OK;

sharemind_executable_common_header_read_error:

    return err;
}
