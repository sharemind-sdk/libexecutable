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

#include <cstddef>
#include <memory>
#include <istream>
#include <ostream>
#include <sharemind/Exception.h>
#include <sharemind/ExceptionMacros.h>
#include <sharemind/codeblock.h>
#include <string>
#include <type_traits>
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
    SHAREMIND_DECLARE_EXCEPTION_NOINLINE(Exception,
                                         DeserializationException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            DeserializationException,
            FailedToDeserializeFileHeaderException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_MSG_NOINLINE(
            DeserializationException,
            FailedToDeserializeFileHeader0x0Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToDeserializeLinkingUnitHeader0x0Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToDeserializeSectionHeader0x0Exception);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleTextSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleRoDataSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleRwDataSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleBssSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleSyscallBindSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultiplePdBindSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            MultipleDebugSectionsInLinkingUnitException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadTextSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadRoDataSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadRwDataSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadBindSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadPdBindSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadDebugSectionDataException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            FailedToReadZeroPaddingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            InvalidZeroPaddingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            DuplicateSyscallBindingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            DuplicatePdBindingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            EmptySyscallBindingException);
    SHAREMIND_DECLARE_EXCEPTION_CONST_STDSTRING_NOINLINE(
            DeserializationException,
            EmptyPdBindingException);

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

    /* Types: */

        using Container = std::vector<SharemindCodeBlock>;

    /* Methods: */

        TextSection()
                noexcept(
                    std::is_nothrow_default_constructible<Container>::value);
        TextSection(TextSection &&)
                noexcept(std::is_nothrow_move_constructible<Container>::value);
        TextSection(TextSection const &);
        TextSection(Container instructions_)
                noexcept(std::is_nothrow_move_constructible<Container>::value);

        TextSection & operator=(TextSection &&)
                noexcept(std::is_nothrow_move_assignable<Container>::value);
        TextSection & operator=(TextSection const &);

    /* Fields: */

        Container instructions;

    };

    struct SyscallBindingsSection {

    /* Types: */

        using Container = std::vector<std::string>;

    /* Methods: */

        SyscallBindingsSection()
                noexcept(
                    std::is_nothrow_default_constructible<Container>::value);
        SyscallBindingsSection(SyscallBindingsSection &&)
                noexcept(std::is_nothrow_move_constructible<Container>::value);
        SyscallBindingsSection(SyscallBindingsSection const &);
        SyscallBindingsSection(Container bindings)
                noexcept(std::is_nothrow_move_constructible<Container>::value);

        SyscallBindingsSection & operator=(SyscallBindingsSection &&)
                noexcept(std::is_nothrow_move_assignable<Container>::value);
        SyscallBindingsSection & operator=(SyscallBindingsSection const &);

    /* Fields: */

        Container syscallBindings;

    };

    struct PdBindingsSection {

    /* Types: */

        using Container = std::vector<std::string>;

    /* Methods: */

        PdBindingsSection()
                noexcept(
                    std::is_nothrow_default_constructible<Container>::value);
        PdBindingsSection(PdBindingsSection &&)
                noexcept(std::is_nothrow_move_constructible<Container>::value);
        PdBindingsSection(PdBindingsSection const &);
        PdBindingsSection(Container pdBindings_)
                noexcept(std::is_nothrow_move_constructible<Container>::value);

        PdBindingsSection & operator=(PdBindingsSection &&)
                noexcept(std::is_nothrow_move_assignable<Container>::value);
        PdBindingsSection & operator=(PdBindingsSection const &);

    /* Fields: */

        Container pdBindings;

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

    using LuContainer = std::vector<LinkingUnit>;

/* Methods: */

    Executable()
            noexcept(std::is_nothrow_default_constructible<LuContainer>::value);
    Executable(Executable &&)
            noexcept(std::is_nothrow_move_constructible<LuContainer>::value);
    Executable(Executable const &);

    Executable & operator=(Executable &&)
            noexcept(std::is_nothrow_move_assignable<LuContainer>::value);
    Executable & operator=(Executable const &);

/* Fields: */

    std::size_t fileFormatVersion = 0x0;
    LuContainer linkingUnits;
    std::size_t activeLinkingUnitIndex = 0u;

};

} /* namespace sharemind { */

std::ostream & operator<<(std::ostream & os, sharemind::Executable const & ex);

std::istream & operator>>(std::istream & is, sharemind::Executable & ex);

#endif /* SHAREMIND_LIBEXECUTABLE_EXECUTABLE_H */
