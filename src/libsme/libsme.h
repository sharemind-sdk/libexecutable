/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef LIBSME_LIBSME_H
#define LIBSME_LIBSME_H

#include <stdint.h>
#include "../static_assert.h"
#include "sme_read_error.h"
#include "sme_section_type.h"


#define SME_VERSION_SUPPORTED 0x0u

struct SME_Common_Header {
    char magic[32];
    uint64_t byte_order_verification __attribute__ ((packed));
    uint16_t file_format_version __attribute__ ((packed));
};
SM_STATIC_ASSERT(sizeof(struct SME_Common_Header) == 32 + 8 + 2);

void SME_Common_Header_init(struct SME_Common_Header * h, uint16_t version);
enum SME_Read_Error SME_Common_Header_read(const void * from, const struct SME_Common_Header ** h)  __attribute__ ((nonnull(1), warn_unused_result));

#endif /* LIBSME_LIBSME_H */
