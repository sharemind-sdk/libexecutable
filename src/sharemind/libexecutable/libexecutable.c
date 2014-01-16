/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "libexecutable.h"

#include <assert.h>
#include <stddef.h>


static const char magic[32] = "Sharemind Executable";

void SharemindExecutableCommonHeader_init(SharemindExecutableCommonHeader * h, uint16_t version) {
    __builtin_memcpy(h->magic, magic, 32);
    h->byteOrderVerification = 0x0123456789abcdef;
    h->fileFormatVersion = version;
    __builtin_bzero(h->zeroPadding, 6);
}

SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableCommonHeader_read(const void * from, const SharemindExecutableCommonHeader ** h) {
    assert(from);

    union {
        const void * v;
        const SharemindExecutableCommonHeader * h;
    } c = { .v = from };
    SHAREMIND_EXECUTABLE_READ_ERROR err;

    if (__builtin_memcmp(c.h->magic, magic, 32) != 0) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_MAGIC;
        goto sharemind_executable_common_header_read_error;
    }

    static const unsigned char byteOrderVerification[8] = { 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01 };
    if (__builtin_memcmp(&c.h->byteOrderVerification, byteOrderVerification, 8) != 0) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_BYTE_ORDER_VERIFICATION;
        goto sharemind_executable_common_header_read_error;
    }

    if (c.h->fileFormatVersion > SHAREMIND_EXECUTABLE_VERSION_SUPPORTED) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_VERSION_NOT_SUPPORTED;
        goto sharemind_executable_common_header_read_error;
    }

    static const uint8_t zeroPadding[6u] = { 0u, 0u, 0u, 0u, 0u, 0u };
    if (__builtin_memcmp(zeroPadding, c.h->zeroPadding, 6) != 0) {
        err = SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA;
        goto sharemind_executable_common_header_read_error;
    }

    if (h)
        (*h) = c.h;
    return SHAREMIND_EXECUTABLE_READ_OK;

sharemind_executable_common_header_read_error:

    if (h)
        (*h) = NULL;
    return err;
}
