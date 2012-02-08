/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef LIBSME_SME_READ_ERROR_H
#define LIBSME_SME_READ_ERROR_H

typedef enum {
    SME_READ_OK = 0,
    SME_READ_ERROR_MAGIC,
    SME_READ_ERROR_BYTE_ORDER_VERIFICATION,
    SME_READ_ERROR_VERSION_NOT_SUPPORTED,
    SME_READ_ERROR_INVALID_DATA
} SME_Read_Error;

#endif /* LIBSME_SME_READ_ERROR_H */
