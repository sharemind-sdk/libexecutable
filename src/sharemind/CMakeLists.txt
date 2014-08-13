#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBEXECUTABLE" "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindVmGlobals REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
                    ${SharemindVmGlobals_INCLUDE_DIRS})

ADD_COMPILE_OPTIONS(
  "-std=c99" "-O2" "-fwrapv" "-Wall" "-Wextra" "-Wunused" "-Wabi"
  "-Wstrict-aliasing=1" "-Wbad-function-cast" "-Wformat" "-Wswitch-default"
  "-Wfloat-equal" "-Wshadow" "-Wpointer-arith" "-Wcast-qual"
  "-Wstrict-prototypes" "-Wconversion" "-Wsign-conversion" "-Wunused-macros"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-ggdb>"
  "$<$<NOT:$<STREQUAL:$<CONFIGURATION>,Release>>:-fno-omit-frame-pointer>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-DNDEBUG>"
  "$<$<STREQUAL:$<CONFIGURATION>,Release>:-fomit-frame-pointer>"
)

INCLUDE(CheckCCompilerFlag)
FUNCTION(CHECK_ADD_COMPILER_FLAG flag)
    STRING(SUBSTRING "${flag}" 1 -1 FlagName)
    CHECK_C_COMPILER_FLAG("${flag}" C_COMPILER_HAS_${FlagName}_FLAG)
    IF(C_COMPILER_HAS_${FlagName}_FLAG)
        ADD_COMPILE_OPTIONS("${flag}")
    ENDIF()
ENDFUNCTION()
CHECK_ADD_COMPILER_FLAG("-Weverything")
CHECK_ADD_COMPILER_FLAG("-Wlogical-op")

SET(LIBEXECUTABLE_INSTALL_DEFINITIONS "-D__STDC_LIMIT_MACROS"
                                      "-D__STDC_CONSTANT_MACROS"
                                      "-D_BSD_SOURCE")
ADD_DEFINITIONS(${SharemindVmGlobals_DEFINITIONS}
                ${LIBEXECUTABLE_INSTALL_DEFINITIONS})


################################################################################
# libexecutable
################################################################################

SET(SHAREMIND_LIBEXECUTABLE_HEADERS
    "libexecutable/endianmacros.h"
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

SET(LIBEXECUTABLE_VERSION "0.1.0.0")
SET_TARGET_PROPERTIES("libexecutable" PROPERTIES
                      OUTPUT_NAME "sharemind_executable"
                      VERSION "${LIBEXECUTABLE_VERSION}"
                      SOVERSION "0.1")

INSTALL(TARGETS "libexecutable"
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBEXECUTABLE_HEADERS}
        DESTINATION "include/sharemind/libexecutable")

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libexecutable_in_files "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libexecutable_in_files_in_IDE" SOURCES ${sharemind_libexecutable_in_files})
SET(LIBEXECUTABLE_INSTALL_LINK_LIBRARIES "-L${CMAKE_INSTALL_PREFIX}/lib" "-lsharemind_executable")
SET(LIBEXECUTABLE_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibexecutableConfig.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfig.cmake" @ONLY)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibexecutableConfigVersion.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfigVersion.cmake" @ONLY)
INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfigVersion.cmake"
        DESTINATION "lib/SharemindLibexecutable")
