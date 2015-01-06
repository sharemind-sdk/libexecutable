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
