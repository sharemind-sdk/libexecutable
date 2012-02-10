/*
 * This file is a part of the Sharemind framework.
 * Copyright (C) Cybernetica AS
 *
 * All rights are reserved. Reproduction in whole or part is prohibited
 * without the written consent of the copyright owner. The usage of this
 * code is subject to the appropriate license agreement.
 */

#ifndef SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_READ_ERROR_H
#define SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_READ_ERROR_H

typedef enum {
    SHAREMIND_EXECUTABLE_READ_OK = 0,
    SHAREMIND_EXECUTABLE_READ_ERROR_MAGIC,
    SHAREMIND_EXECUTABLE_READ_ERROR_BYTE_ORDER_VERIFICATION,
    SHAREMIND_EXECUTABLE_READ_ERROR_VERSION_NOT_SUPPORTED,
    SHAREMIND_EXECUTABLE_READ_ERROR_INVALID_DATA
} SHAREMIND_EXECUTABLE_READ_ERROR;

#endif /* SHAREMIND_LIBEXECUTABLE_SHAREMIND_EXECUTABLE_READ_ERROR_H */
