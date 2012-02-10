/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H

#include <sharemind/static_assert.h>
#include <stdint.h>
#include "sharemind_executable_read_error.h"
#include "sharemind_executable_section_type.h"


#define SHAREMIND_EXECUTABLE_VERSION_SUPPORTED 0x0u

typedef struct __attribute__ ((__packed__)) {
    char magic[32];
    uint64_t byteOrderVerification;
    uint16_t fileFormatVersion;
} SharemindExecutableCommonHeader;
SHAREMIND_STATIC_ASSERT(sizeof(SharemindExecutableCommonHeader) == 32 + 8 + 2);

void SharemindExecutableCommonHeader_init(SharemindExecutableCommonHeader * h, uint16_t version);
SHAREMIND_EXECUTABLE_READ_ERROR SharemindExecutableCommonHeader_read(const void * from, const SharemindExecutableCommonHeader ** h)  __attribute__ ((nonnull(1), warn_unused_result));

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H */
