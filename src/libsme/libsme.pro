#
# This file is a part of the Sharemind framework.
# Copyright (C) Cybernetica AS
#
# All rights are reserved. Reproduction in whole or part is prohibited
# without the written consent of the copyright owner. The usage of this
# code is subject to the appropriate license agreement.
#

include(../../vm.pri)

TEMPLATE = lib
TARGET = sme
DESTDIR = ../../lib/

SOURCES += \
    libsme.c \
    libsme_0x0.c

OTHER_FILES += \
    libsme_0x0.h \
    libsme.h \
    sme_read_error.h \
    sme_section_type.h
