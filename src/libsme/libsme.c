/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#include "libsme.h"

#include <assert.h>
#include <stddef.h>


static const char magic[32] = "Sharemind Executable";

void SME_Common_Header_init(struct SME_Common_Header * h, uint16_t version) {
    __builtin_memcpy(h->magic, magic, 32);
    h->byte_order_verification = 0x0123456789abcdef;
    h->file_format_version = version;
}

enum SME_Read_Error SME_Common_Header_read(const void * from, const struct SME_Common_Header ** h) {
    assert(from);

    union {
        const void * v;
        const struct SME_Common_Header * h;
    } c = { .v = from };
    enum SME_Read_Error err;

    if (__builtin_memcmp(c.h->magic, magic, 32) != 0) {
        err = SME_READ_ERROR_MAGIC;
        goto SME_Common_Header_read_error;
    }

    static const char byte_order_verification[8] = "\xef\xcd\xab\x89\x67\x45\x23\x01";
    if (__builtin_memcmp(&c.h->byte_order_verification, byte_order_verification, 8) != 0) {
        err = SME_READ_ERROR_BYTE_ORDER_VERIFICATION;
        goto SME_Common_Header_read_error;
    }

    if (c.h->file_format_version > SME_VERSION_SUPPORTED) {
        err = SME_READ_ERROR_VERSION_NOT_SUPPORTED;
        goto SME_Common_Header_read_error;
    }

    if (h)
        (*h) = c.h;
    return SME_READ_OK;

SME_Common_Header_read_error:

    if (h)
        (*h) = NULL;
    return err;
}
