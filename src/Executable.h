/*
 * Copyright (C) Cybernetica
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

#ifndef SHAREMIND_LIBEXECUTABLE_EXECUTABLE_H
#define SHAREMIND_LIBEXECUTABLE_EXECUTABLE_H

#include <memory>
#include <ostream>
#include <sharemind/Exception.h>
#include <sharemind/ExceptionMacros.h>
#include <sharemind/codeblock.h>
#include <string>
#include <vector>


namespace sharemind {

struct Executable {

/* Types: */

    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(sharemind::Exception, Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            Exception,
            FormatVersionNotSupportedException);
    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(Exception, NotSerializableException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            NoLinkingUnitsDefinedException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            TooManyLinkingUnitsDefinedException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            InvalidActiveLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            NoSectionsDefinedInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            TooManySectionsDefinedInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            TextSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            RoDataSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            RwDataSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            BssSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            BindingsSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            PdBindingsSectionTooBigException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            NotSerializableException,
            DebugSectionTooBigException);

    struct BssSection {

    /* Methods: */

        BssSection() noexcept {}
        BssSection(BssSection &&) noexcept = default;
        BssSection(BssSection const &) noexcept = default;
        BssSection(std::size_t newSizeInBytes) noexcept
            : sizeInBytes(newSizeInBytes)
        {}

        BssSection & operator=(BssSection &&) noexcept = default;
        BssSection & operator=(BssSection const &) noexcept = default;

    /* Fields */

        std::size_t sizeInBytes = 0u;

    };

    struct DataSection {

    /* Types: */

        enum CopyDataTag { CopyData };

    /* Methods: */

        DataSection() noexcept;

        DataSection(std::shared_ptr<void> dataPtr,
                    std::size_t dataSize) noexcept;

        DataSection(void const * dataPtr,
                    std::size_t dataSize,
                    CopyDataTag const);

        DataSection(DataSection &&) noexcept;
        DataSection(DataSection const &);

        DataSection & operator=(DataSection &&) noexcept;
        DataSection & operator=(DataSection const &);

    /* Fields: */

        std::shared_ptr<void> data;
        std::size_t sizeInBytes = 0u;

    };

    struct TextSection {

    /* Methods: */

        TextSection() noexcept;
        TextSection(TextSection &&) noexcept;
        TextSection(TextSection const &);
        TextSection(std::vector<SharemindCodeBlock> instructions_) noexcept;

        TextSection & operator=(TextSection &&) noexcept;
        TextSection & operator=(TextSection const &);

    /* Fields: */

        std::vector<SharemindCodeBlock> instructions;

    };

    struct SyscallBindingsSection {

    /* Methods: */

        SyscallBindingsSection() noexcept;
        SyscallBindingsSection(SyscallBindingsSection &&) noexcept;
        SyscallBindingsSection(SyscallBindingsSection const &);
        SyscallBindingsSection(std::vector<std::string> bindings) noexcept;

        SyscallBindingsSection & operator=(SyscallBindingsSection &&) noexcept;
        SyscallBindingsSection & operator=(SyscallBindingsSection const &);

    /* Fields: */

        std::vector<std::string> syscallBindings;

    };

    struct PdBindingsSection {

    /* Methods: */

        PdBindingsSection() noexcept;
        PdBindingsSection(PdBindingsSection &&) noexcept;
        PdBindingsSection(PdBindingsSection const &);
        PdBindingsSection(std::vector<std::string> pdBindings_) noexcept;

        PdBindingsSection & operator=(PdBindingsSection &&) noexcept;
        PdBindingsSection & operator=(PdBindingsSection const &);

    /* Fields: */

        std::vector<std::string> pdBindings;

    };

    struct LinkingUnit {

    /* Methods: */

        LinkingUnit() noexcept;
        LinkingUnit(LinkingUnit &&) noexcept;
        LinkingUnit(LinkingUnit const &);

        LinkingUnit & operator=(LinkingUnit &&) noexcept;
        LinkingUnit & operator=(LinkingUnit const &);

        std::size_t numberOfSections() const noexcept;

    /* Fields: */

        std::shared_ptr<TextSection> textSection;
        std::shared_ptr<DataSection> roDataSection;
        std::shared_ptr<DataSection> rwDataSection;
        std::shared_ptr<BssSection> bssSection;
        std::shared_ptr<SyscallBindingsSection> syscallBindingsSection;
        std::shared_ptr<PdBindingsSection> pdBindingsSection;
        std::shared_ptr<DataSection> debugSection;

    };

/* Methods: */

    Executable() noexcept;
    Executable(Executable &&) noexcept;
    Executable(Executable const &);

    Executable & operator=(Executable &&) noexcept;
    Executable & operator=(Executable const &);

    std::ostream & serializeToStream(std::ostream & os,
                                     std::size_t formatVersion = 0u) const;

/* Fields: */

    std::vector<LinkingUnit> linkingUnits;
    std::size_t activeLinkingUnit = 0u;

};

} /* namespace sharemind { */

std::ostream & operator<<(std::ostream & os, sharemind::Executable const & ex)
{ return ex.serializeToStream(os); }

#endif /* SHAREMIND_LIBEXECUTABLE_EXECUTABLE_H */
