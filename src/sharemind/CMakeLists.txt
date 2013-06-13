#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8")
PROJECT("SHAREMIND_LIBEXECUTABLE" "C" "CXX")

# Find boost - seems to be required since the variables aren't populated otherwise
FIND_PACKAGE(Boost REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

SET(CMAKE_C_FLAGS "-std=c99 -Wall -Wextra -O2 -fstrict-aliasing -Wstrict-aliasing=1 -fwrapv -Wabi -Wbad-function-cast -Wc++-compat -Wformat -Wswitch-default -Wunused -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wconversion -Wsign-conversion -Wunused-macros")

INCLUDE(CheckCCompilerFlag)
MACRO(FLAGS_MAGIC flags)
    CHECK_C_COMPILER_FLAG("-${flags}" SHAREMIND_HAS_CFLAGS__${flags})
    IF(SHAREMIND_HAS_CFLAGS__${flags})
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -${flags}")
    ENDIF(SHAREMIND_HAS_CFLAGS__${flags})
ENDMACRO(FLAGS_MAGIC)
FLAGS_MAGIC("Weverything")
FLAGS_MAGIC("Wlogical-op")

ADD_DEFINITIONS("-D_XOPEN_SOURCE=500" "-D__STDC_LIMIT_MACROS")

################################################################################
# libexecutable
################################################################################


SET(SHAREMIND_LIBEXECUTABLE_HEADERS
    "libexecutable/libexecutable_0x0.h"
    "libexecutable/libexecutable.h"
    "libexecutable/sharemind_executable_read_error.h"
    "libexecutable/sharemind_executable_section_type.h")

IF(NOT SHAREMIND_LIBEXECUTABLE_LINKING_MODE)
    SET(SHAREMIND_LIBEXECUTABLE_LINKING_MODE SHARED)
ENDIF()

ADD_LIBRARY("libexecutable" ${SHAREMIND_LIBEXECUTABLE_LINKING_MODE}
            "libexecutable/libexecutable.c"
            "libexecutable/libexecutable_0x0.c"
            ${SHAREMIND_LIBEXECUTABLE_HEADERS})

SET_TARGET_PROPERTIES("libexecutable" PROPERTIES
                      OUTPUT_NAME "sharemind_executable"
                      VERSION "0.1.0.0"
                      SOVERSION "0.1")

INSTALL(TARGETS "libexecutable"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBEXECUTABLE_HEADERS}
        DESTINATION "include/sharemind/libexecutable")
