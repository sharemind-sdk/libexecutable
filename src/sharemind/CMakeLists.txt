#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8")
CMAKE_POLICY(SET CMP0015 NEW)
PROJECT("SHAREMIND_VM_LIBEXECUTABLE" "C" "CXX")

IF(NOT SHAREMIND_LIB_DIR)
    MESSAGE(FATAL_ERROR "SHAREMIND_LIB_DIR not defined!")
ENDIF(NOT SHAREMIND_LIB_DIR)

IF(NOT SHAREMIND_INCLUDE_DESTINATION)
    MESSAGE(FATAL_ERROR "SHAREMIND_INCLUDE_DESTINATION not defined!")
ENDIF(NOT SHAREMIND_INCLUDE_DESTINATION)

IF(NOT SHAREMIND_EXT_VM_GLOBALS_DIR)
    SET(SHAREMIND_EXT_VM_GLOBALS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../ext/vm_globals")
ENDIF(NOT SHAREMIND_EXT_VM_GLOBALS_DIR)
IF(NOT IS_DIRECTORY "${SHAREMIND_EXT_VM_GLOBALS_DIR}/include/sharemind")
    MESSAGE(FATAL "SHAREMIND_EXT_VM_GLOBALS_DIR is invalid!")
ENDIF()
INCLUDE_DIRECTORIES(BEFORE SYSTEM "${CMAKE_CURRENT_SOURCE_DIR}/../sharemind/.." "${SHAREMIND_EXT_VM_GLOBALS_DIR}/include")


SET(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -O2 -fstrict-aliasing -Wstrict-aliasing=1 -fwrapv -Wabi -Wbad-function-cast -Wc++-compat -Wformat -Wswitch-default -Wunused -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wconversion -Wsign-conversion -Wlogical-op -Wunused-macros")
SET(CMAKE_CXX_FLAGS "-std=gnu++98 -Wall -Wextra -O2 -fstrict-aliasing -Wstrict-aliasing=1 -fwrapv -Wabi -Wformat -Wswitch-default -Wunused -Wfloat-equal -Wshadow -Wpointer-arith -Wcast-qual -Wconversion -Wsign-conversion -Wlogical-op -Wunused-macros")
ADD_DEFINITIONS("-D_XOPEN_SOURCE=500" "-D__STDC_LIMIT_MACROS")

SET(OTHER_FILES "")
FUNCTION(PREPARE_OTHER_FILES)
    FOREACH(OTHER_FILE ${OTHER_FILES})
        SET(P ${P} "${OTHER_FILE}")
    ENDFOREACH(OTHER_FILE ${OTHER_FILES})
    SET(PREPARED_OTHER_FILES ${PREPARED_OTHER_FILES} ${P} PARENT_SCOPE)
ENDFUNCTION(PREPARE_OTHER_FILES)


################################################################################
# libexecutable
################################################################################


SET(OTHER_FILES ${OTHER_FILES}
    "libexecutable/libexecutable_0x0.h"
    "libexecutable/libexecutable.h"
    "libexecutable/sharemind_executable_read_error.h"
    "libexecutable/sharemind_executable_section_type.h")

PREPARE_OTHER_FILES()
ADD_LIBRARY("libexecutable" SHARED
            "libexecutable/libexecutable.c"
            "libexecutable/libexecutable_0x0.c"
            ${PREPARED_OTHER_FILES}) # <-- hack for Qt Creator to include other files)

SET_TARGET_PROPERTIES("libexecutable" PROPERTIES
                      OUTPUT_NAME "sharemind_executable"
                      LIBRARY_OUTPUT_DIRECTORY "${SHAREMIND_LIB_DIR}"
                      VERSION "0.1.0.0"
                      SOVERSION "0.1")
INSTALL(DIRECTORY "libexecutable/"
        DESTINATION "${SHAREMIND_INCLUDE_DESTINATION}/libexecutable"
        FILES_MATCHING PATTERN "*.h")

INSTALL(TARGETS "libexecutable"
        RUNTIME DESTINATION "bin"
        LIBRARY DESTINATION "lib"
        PUBLIC_HEADER DESTINATION "include")
