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

CMAKE_MINIMUM_REQUIRED(VERSION "3.0")
PROJECT(SharemindLibExecutable VERSION "0.1.0" LANGUAGES "C")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers REQUIRED)

SharemindSetProjectVersion()

FIND_PACKAGE(Boost REQUIRED)
FIND_PACKAGE(SharemindCHeaders REQUIRED)


################################################################################
# libexecutable
################################################################################

FILE(GLOB_RECURSE SharemindLibExecutable_HEADERS
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
FILE(GLOB_RECURSE SharemindLibExecutable_SOURCES
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
SharemindSetC99CompileOptions(COMPILE_FLAGS "-fwrapv")

SharemindAddSharedLibrary("libexecutable"
    SOURCES ${SharemindLibExecutable_SOURCES} ${SharemindLibExecutable_HEADERS}
    OUTPUT_NAME "sharemind_executable"
    INCLUDE_DIRECTORIES
        ${Boost_INCLUDE_DIRS}
        ${SharemindCHeaders_INCLUDE_DIRS}
    COMPILE_DEFINITIONS "_BSD_SOURCE" "_DEFAULT_SOURCE"
    LEGACY_DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
)

INSTALL(FILES ${SharemindLibExecutable_HEADERS}
        DESTINATION "include/sharemind/libexecutable"
        COMPONENT dev)

# Install cmake files:
SharemindCreateCMakeFindFiles(
    INCLUDE_DIRS
        "${CMAKE_INSTALL_PREFIX}/include"
        ${Boost_INCLUDE_DIRS}
        ${SharemindCHeaders_INCLUDE_DIRS}
    DEFINITIONS
        ${SharemindCHeaders_DEFINITIONS}
        "-D__STDC_LIMIT_MACROS"
        "-D__STDC_CONSTANT_MACROS"
        "-D_BSD_SOURCE"
        "-D_DEFAULT_SOURCE"
    LIBRARIES
        "-L${CMAKE_INSTALL_PREFIX}/lib"
        "-lsharemind_executable"
)


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-executable"
    DESCRIPTION "Sharemind Executable library"
    DEB_SECTION "libs"
    DEB_DEPENDS "libc6 (>= 2.19)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-executable-dev"
    DESCRIPTION "Sharemind Executable library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-executable (= ${SharemindLibExecutable_VERSION})"
        "libsharemind-cheaders-dev (>= 0.1.0)"
        "libc6-dev (>= 2.19)"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-executable-dbg"
    DESCRIPTION "Sharemind Executable library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-executable (= ${SharemindLibExecutable_VERSION})"
)
SharemindPackagingFinalize()
