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
PROJECT(SharemindLibExecutable VERSION 0.4.0 LANGUAGES "CXX")

INCLUDE("${CMAKE_CURRENT_SOURCE_DIR}/config.local" OPTIONAL)
INCLUDE("${CMAKE_CURRENT_BINARY_DIR}/config.local" OPTIONAL)

FIND_PACKAGE(SharemindCMakeHelpers 1.6 REQUIRED)

FIND_PACKAGE(SharemindCxxHeaders 0.8.0 REQUIRED)


# LibExecutable:
FILE(GLOB_RECURSE SharemindLibExecutable_HEADERS
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
FILE(GLOB_RECURSE SharemindLibExecutable_SOURCES
                  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
SharemindSetCxx14CompileOptions()
SharemindAddSharedLibrary(LibExecutable
    SOURCES ${SharemindLibExecutable_SOURCES} ${SharemindLibExecutable_HEADERS}
    OUTPUT_NAME "sharemind_executable"
)
TARGET_INCLUDE_DIRECTORIES(LibExecutable
    INTERFACE
        # $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src> # TODO
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    )
TARGET_LINK_LIBRARIES(LibExecutable PUBLIC Sharemind::CxxHeaders)
INSTALL(FILES ${SharemindLibExecutable_HEADERS}
        DESTINATION "include/sharemind/libexecutable"
        COMPONENT dev)
SharemindCreateCMakeFindFilesForTarget(LibExecutable
    DEPENDENCIES
        "SharemindCxxHeaders 0.8.0"
    )


# Packaging:
SharemindSetupPackaging()
SharemindAddComponentPackage("lib"
    NAME "libsharemind-executable"
    DESCRIPTION "Sharemind Executable library"
    DEB_SECTION "libs"
    DEB_DEPENDS
        "libc6 (>= 2.19)"
        "libstdc++6 (>= 4.8.0)"
)
SharemindAddComponentPackage("dev"
    NAME "libsharemind-executable-dev"
    DESCRIPTION "Sharemind Executable library development headers"
    DEB_SECTION "libdevel"
    DEB_DEPENDS
        "libsharemind-executable (= ${SharemindLibExecutable_DEB_lib_PACKAGE_VERSION})"
        "libsharemind-cxxheaders-dev (>= 0.8.0)"
        "libc6-dev (>= 2.19)"
        "libstdc++-dev"
)
SharemindAddComponentPackage("debug"
    NAME "libsharemind-executable-dbg"
    DESCRIPTION "Sharemind Executable library debug symbols"
    DEB_SECTION "debug"
    DEB_DEPENDS "libsharemind-executable (= ${SharemindLibExecutable_DEB_lib_PACKAGE_VERSION})"
)
SharemindPackagingFinalize()
