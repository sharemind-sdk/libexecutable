#
# Copyright (C) 2015 Cybernetica
#
# Research/Commercial License Usage
# Licensees holding a valid Research License or Commercial License
# for the Software may use this file according to the written
# agreement between you and Cybernetica.
#
# GNU General Public License Usage
# Alternatively, this file may be used under the terms of the GNU
# General Public License version 3.0 as published by the Free Software
# Foundation and appearing in the file LICENSE.GPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU General Public License version 3.0 requirements will be
# met: http://www.gnu.org/copyleft/gpl-3.0.html.
#
# For further information, please contact us at sharemind@cyber.ee.
#

CMAKE_MINIMUM_REQUIRED(VERSION "2.8.12")
PROJECT("SHAREMIND_LIBEXECUTABLE" "C")

SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(LIBEXECUTABLE_VERSION
    "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS}
                    ${SharemindCHeaders_INCLUDE_DIRS})

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
                                      "-D_BSD_SOURCE"
                                      "-D_DEFAULT_SOURCE")
ADD_DEFINITIONS(${SharemindCHeaders_DEFINITIONS}
                ${LIBEXECUTABLE_INSTALL_DEFINITIONS})


################################################################################
# libexecutable
################################################################################

FILE(GLOB_RECURSE SHAREMIND_LIBEXECUTABLE_HEADERS
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
FILE(GLOB_RECURSE SHAREMIND_LIBEXECUTABLE_SOURCES
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
IF(NOT SHAREMIND_LIBEXECUTABLE_LINKING_MODE)
    SET(SHAREMIND_LIBEXECUTABLE_LINKING_MODE SHARED)
ENDIF()
ADD_LIBRARY("libexecutable" ${SHAREMIND_LIBEXECUTABLE_LINKING_MODE}
            ${SHAREMIND_LIBEXECUTABLE_SOURCES}
            ${SHAREMIND_LIBEXECUTABLE_HEADERS})

SET_TARGET_PROPERTIES("libexecutable" PROPERTIES
                      OUTPUT_NAME "sharemind_executable"
                      VERSION "${LIBEXECUTABLE_VERSION}"
                      SOVERSION "0.1")

INSTALL(TARGETS "libexecutable"
        COMPONENT lib
        LIBRARY DESTINATION "lib"
        ARCHIVE DESTINATION "lib")

INSTALL(FILES ${SHAREMIND_LIBEXECUTABLE_HEADERS}
        DESTINATION "include/sharemind/libexecutable"
        COMPONENT dev)

# Install cmake files:
FILE(GLOB_RECURSE sharemind_libexecutable_in_files
                  "${CMAKE_CURRENT_SOURCE_DIR}/*.in")
ADD_CUSTOM_TARGET("include_sharemind_libexecutable_in_files_in_IDE"
                  SOURCES ${sharemind_libexecutable_in_files})
SET(LIBEXECUTABLE_INSTALL_LINK_LIBRARIES "-L${CMAKE_INSTALL_PREFIX}/lib"
                                         "-lsharemind_executable")
SET(LIBEXECUTABLE_INSTALL_INCLUDE_DIRS "${CMAKE_INSTALL_PREFIX}/include")

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibexecutableConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfig.cmake" @ONLY)
CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/SharemindLibexecutableConfigVersion.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfigVersion.cmake"
    @ONLY)
INSTALL(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/SharemindLibexecutableConfigVersion.cmake"
    DESTINATION "lib/SharemindLibexecutable"
    COMPONENT dev)


# Packaging:
SET(CPACK_PACKAGE_VENDOR "Cybernetica AS")
SET(CPACK_PACKAGE_CONTACT "Sharemind packaging <sharemind-packaging@cyber.ee>")
SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_NAME "libsharemind-executable")
SET(CPACK_DEBIAN_PACKAGE_RELEASE "cyber1")
SET(CPACK_DEBIAN_COMPRESSION_TYPE "xz")
SET(CPACK_DEB_COMPONENT_INSTALL "ON")
SET(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")

SET(CPACK_DEBIAN_LIB_PACKAGE_NAME "libsharemind-executable")
SET(CPACK_COMPONENT_DEV_DESCRIPTION
    "Sharemind Executable library development headers")
SET(CPACK_COMPONENT_LIB_DESCRIPTION "Sharemind Executable library")
SET(CPACK_DEBIAN_DEV_PACKAGE_DEPENDS "libsharemind-executable (= ${LIBEXECUTABLE_VERSION}), libsharemind-cheaders-dev (>= 0.1.0), libc6-dev (>= 2.19)")
SET(CPACK_DEBIAN_LIB_PACKAGE_DEPENDS "libc6 (>= 2.19)")
SET(CPACK_DEBIAN_DEV_PACKAGE_SECTION "libdevel")
SET(CPACK_DEBIAN_LIB_PACKAGE_SECTION "libs")
SET(CPACK_DEBIAN_DEV_PACKAGE_ARCHITECTURE "any")
INCLUDE(CPack)
