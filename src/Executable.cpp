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

#include "Executable.h"

#include <cstring>
#include <limits>
#include <sharemind/compiler-support/GccNoreturn.h>
#include <sharemind/Concat.h>
#include <sharemind/GlobalDeleter.h>
#include <type_traits>
#include <utility>
#include "libexecutable.h"
#include "libexecutable_0x0.h"


namespace sharemind {
namespace {

template <typename Exception>
struct ThrowingBindingsSizeOverflowCheck {

    template <typename BindingSize, typename SizeLeft>
    void operator()(BindingSize bindingSize, SizeLeft sizeLeft) {
        if (bindingSize > sizeLeft)
            throw Exception();
    }

};

struct AssertingBindingsSizeOverflowCheck {

    template <typename BindingSize, typename SizeLeft>
    void operator()(BindingSize bindingSize, SizeLeft sizeLeft)
    { assert(bindingSize <= sizeLeft); }

};

template <typename BindingsSizeOverflowCheck,
          typename BindingsContainer>
std::size_t calculateBindingsSize(BindingsContainer const & bindings) {
    std::size_t r = bindings.size();
    std::size_t sizeLeft = std::numeric_limits<std::size_t>::max() - r;
    for (auto const & binding : bindings) {
        auto const bindingSize = binding.size();
        BindingsSizeOverflowCheck()(bindingSize, sizeLeft);
        r += binding.size();
        sizeLeft -= binding.size();
    }
    return r;
}

SHAREMIND_GCC_NORETURN_PART1
void throwFormatVersionNotSupportedException(std::size_t formatVersion)
        SHAREMIND_GCC_NORETURN_PART2
{
    throw Executable::FormatVersionNotSupportedException(
                concat("Sharemind Executable file format version ",
                       formatVersion, " not supported!"));
}

template <typename Exception>
void checkSectionSize(std::size_t size) {
    using SS = ExecutableSectionHeader0x0::SizeType;
    if (size > std::numeric_limits<SS>::max())
        throw Exception();
};

std::ostream & serializeSectionHeader(
        std::ostream & os,
        ExecutableSectionHeader0x0::SectionType type,
        std::size_t size) noexcept
{
    using SS = ExecutableSectionHeader0x0::SizeType;
    assert(size <= std::numeric_limits<SS>::max());

    ExecutableSectionHeader0x0 sectionHeader0x0;
    sectionHeader0x0.init(type, static_cast<SS>(size));
    assert(sectionHeader0x0.isValid());
    return os << sectionHeader0x0;
};

constexpr std::streamsize const extraPaddingSize[8u] =
        { 0u, 7u, 6u, 5u, 4u, 3u, 2u, 1u };
char const extraPadding[8u] = "\0\0\0\0\0\0\0";

std::ostream & serializeRegularSection(
        std::ostream & os,
        ExecutableSectionHeader0x0::SectionType type,
        void const * data,
        std::size_t size)
{
    if (!serializeSectionHeader(os, type, size))
        return os;

    static constexpr auto const writePatch =
            std::numeric_limits<std::streamsize>::max();
    auto d = static_cast<char const *>(data);
    while (size > writePatch) {
        if (!os.write(d, writePatch))
            return os;
        size -= writePatch;
        d += writePatch;
    }
    if (!os.write(d, static_cast<std::streamsize>(size)))
        return os;
    return os.write(extraPadding, extraPaddingSize[size % 8u]);
}

std::ostream & serializeBindingsSection(
        std::ostream & os,
        ExecutableSectionHeader0x0::SectionType type,
        std::vector<std::string> const & bindings)
{
    auto const size =
            calculateBindingsSize<AssertingBindingsSizeOverflowCheck>(bindings);
    if (!serializeSectionHeader(os, type, size))
        return os;
    for (auto const & binding : bindings)
        if (!os.write(binding.c_str(),
                      static_cast<std::streamsize>(binding.size() + 1u)))
            return os;
    return os.write(extraPadding, extraPaddingSize[size % 8u]);
}

} // anonymous namespace

SHAREMIND_DEFINE_EXCEPTION_NOINLINE(sharemind::Exception,
                                    Executable::,
                                    Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        Exception,
        Executable::,
        FormatVersionNotSupportedException);
SHAREMIND_DEFINE_EXCEPTION_NOINLINE(Exception,
                                    Executable::,
                                    NotSerializableException);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        NoLinkingUnitsDefinedException,
        "No linking units defined!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        TooManyLinkingUnitsDefinedException,
        "Too many linking units defined!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        InvalidActiveLinkingUnitException,
        "Invalid active linking unit!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        NoSectionsDefinedInLinkingUnitException,
        "No sections defined in linking unit!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        TooManySectionsDefinedInLinkingUnitException,
        "Too many sections defined in linking unit!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        TextSectionTooBigException,
        "Text section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        RoDataSectionTooBigException,
        "Read-only data section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        RwDataSectionTooBigException,
        "Read-write data section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        BssSectionTooBigException,
        "BSS section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        BindingsSectionTooBigException,
        "Bindings section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        PdBindingsSectionTooBigException,
        "Protection domain bindings section is too big to serialize!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        NotSerializableException,
        Executable::,
        DebugSectionTooBigException,
        "Debug section is too big to serialize!");



Executable::DataSection::DataSection() noexcept {}

Executable::DataSection::DataSection(std::shared_ptr<void> dataPtr,
                                     std::size_t dataSizeInBytes) noexcept
    : data(std::move(dataPtr))
    , sizeInBytes(dataSizeInBytes)
{}

Executable::DataSection::DataSection(void const * dataPtr,
                                     std::size_t dataSizeInBytes,
                                     CopyDataTag const)
    : data(::operator new(dataSizeInBytes), GlobalDeleter())
    , sizeInBytes(dataSizeInBytes)
{ std::memcpy(data.get(), dataPtr, dataSizeInBytes); }

Executable::DataSection::DataSection(DataSection &&) noexcept = default;

Executable::DataSection::DataSection(DataSection const & copy)
    : DataSection(copy.data.get(), copy.sizeInBytes, CopyData)
{}

Executable::DataSection & Executable::DataSection::operator=(DataSection &&)
        noexcept = default;

Executable::DataSection & Executable::DataSection::operator=(
        DataSection const & copy)
{
    data = std::shared_ptr<void>(::operator new(copy.sizeInBytes),
                                 GlobalDeleter());
    sizeInBytes = copy.sizeInBytes;
    return *this;
}



Executable::TextSection::TextSection() noexcept = default;

Executable::TextSection::TextSection(TextSection &&) noexcept = default;

Executable::TextSection::TextSection(TextSection const &) = default;

Executable::TextSection::TextSection(
        std::vector<SharemindCodeBlock> instructions_) noexcept
    : instructions(std::move(instructions_))
{}

Executable::TextSection & Executable::TextSection::operator=(TextSection &&)
        noexcept = default;

Executable::TextSection & Executable::TextSection::operator=(
        TextSection const &) = default;



Executable::SyscallBindingsSection::SyscallBindingsSection() noexcept = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(
        SyscallBindingsSection &&) noexcept = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(
        SyscallBindingsSection const &) = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(
        std::vector<std::string> bindings) noexcept
    : syscallBindings(std::move(bindings))
{}

Executable::SyscallBindingsSection &
Executable::SyscallBindingsSection::operator=(SyscallBindingsSection &&)
        noexcept = default;

Executable::SyscallBindingsSection &
Executable::SyscallBindingsSection::operator=(SyscallBindingsSection const &)
        = default;



Executable::PdBindingsSection::PdBindingsSection() noexcept = default;

Executable::PdBindingsSection::PdBindingsSection(PdBindingsSection &&) noexcept
        = default;

Executable::PdBindingsSection::PdBindingsSection(PdBindingsSection const &)
        = default;

Executable::PdBindingsSection::PdBindingsSection(
        std::vector<std::string> pdBindings_) noexcept
    : pdBindings(std::move(pdBindings_))
{}

Executable::PdBindingsSection & Executable::PdBindingsSection::operator=(
        PdBindingsSection &&) noexcept = default;

Executable::PdBindingsSection & Executable::PdBindingsSection::operator=(
        PdBindingsSection const &) = default;



Executable::LinkingUnit::LinkingUnit() noexcept = default;

Executable::LinkingUnit::LinkingUnit(LinkingUnit &&) noexcept = default;

Executable::LinkingUnit::LinkingUnit(LinkingUnit const & copy)
    : textSection(copy.textSection
                  ? std::make_shared<TextSection>(*copy.textSection)
                  : std::shared_ptr<TextSection>())
    , roDataSection(copy.roDataSection
                    ? std::make_shared<DataSection>(*copy.roDataSection)
                    : std::shared_ptr<DataSection>())
    , rwDataSection(copy.rwDataSection
                    ? std::make_shared<DataSection>(*copy.rwDataSection)
                    : std::shared_ptr<DataSection>())
    , bssSection(copy.bssSection
                 ? std::make_shared<BssSection>(*copy.bssSection)
                 : std::shared_ptr<BssSection>())
    , syscallBindingsSection(
        copy.syscallBindingsSection
        ? std::make_shared<SyscallBindingsSection>(*copy.syscallBindingsSection)
        : std::shared_ptr<SyscallBindingsSection>())
    , pdBindingsSection(
          copy.pdBindingsSection
          ? std::make_shared<PdBindingsSection>(*copy.pdBindingsSection)
          : std::shared_ptr<PdBindingsSection>())
    , debugSection(copy.debugSection
                   ? std::make_shared<DataSection>(*copy.debugSection)
                   : std::shared_ptr<DataSection>())
{}

Executable::LinkingUnit & Executable::LinkingUnit::operator=(LinkingUnit &&)
        noexcept = default;

Executable::LinkingUnit & Executable::LinkingUnit::operator=(
        LinkingUnit const & copy)
{
    textSection = copy.textSection
                  ? std::make_shared<TextSection>(*copy.textSection)
                  : std::shared_ptr<TextSection>();
    roDataSection = copy.roDataSection
                    ? std::make_shared<DataSection>(*copy.roDataSection)
                    : std::shared_ptr<DataSection>();
    rwDataSection = copy.rwDataSection
                    ? std::make_shared<DataSection>(*copy.rwDataSection)
                    : std::shared_ptr<DataSection>();
    bssSection = copy.bssSection
                 ? std::make_shared<BssSection>(*copy.bssSection)
                 : std::shared_ptr<BssSection>();
    syscallBindingsSection =
            copy.syscallBindingsSection
            ? std::make_shared<SyscallBindingsSection>(
                  *copy.syscallBindingsSection)
            : std::shared_ptr<SyscallBindingsSection>();
    pdBindingsSection =
          copy.pdBindingsSection
          ? std::make_shared<PdBindingsSection>(*copy.pdBindingsSection)
          : std::shared_ptr<PdBindingsSection>();
    debugSection = copy.debugSection
                   ? std::make_shared<DataSection>(*copy.debugSection)
                   : std::shared_ptr<DataSection>();
    return *this;
}

std::size_t Executable::LinkingUnit::numberOfSections() const noexcept {
    std::size_t r = 0u;
    if (textSection)
        ++r;
    if (roDataSection)
        ++r;
    if (rwDataSection)
        ++r;
    if (bssSection)
        ++r;
    if (syscallBindingsSection)
        ++r;
    if (pdBindingsSection)
        ++r;
    if (debugSection)
        ++r;
    return r;
}


Executable::Executable() noexcept = default;
Executable::Executable(Executable &&) noexcept = default;
Executable::Executable(Executable const &) = default;

Executable & Executable::operator=(Executable &&) noexcept = default;
Executable & Executable::operator=(Executable const &) = default;

std::ostream & Executable::serializeToStream(std::ostream & os,
                                             std::size_t formatVersion) const
{
    if (formatVersion != 0x0)
        throwFormatVersionNotSupportedException(formatVersion);

    if (linkingUnits.empty())
        throw NoLinkingUnitsDefinedException();
    if (linkingUnits.size() - 1u
        > std::numeric_limits<ExecutableHeader0x0::NumLinkingUnitsSize>::max())
        throw TooManyLinkingUnitsDefinedException();
    if (activeLinkingUnit >= linkingUnits.size())
        throw InvalidActiveLinkingUnitException();

    for (auto const & lu : linkingUnits) {
        auto const numberOfSections = lu.numberOfSections();
        if (!numberOfSections)
            throw NoSectionsDefinedInLinkingUnitException();
        using NSS = ExecutableLinkingUnitHeader0x0::NumSectionsSize;
        if (numberOfSections - 1u > std::numeric_limits<NSS>::max())
            throw TooManySectionsDefinedInLinkingUnitException();
        if (lu.textSection)
            checkSectionSize<TextSectionTooBigException>(
                        lu.textSection->instructions.size());
        if (lu.roDataSection)
            checkSectionSize<RoDataSectionTooBigException>(
                        lu.roDataSection->sizeInBytes);
        if (lu.rwDataSection)
            checkSectionSize<RwDataSectionTooBigException>(
                        lu.rwDataSection->sizeInBytes);
        if (lu.bssSection)
            checkSectionSize<BssSectionTooBigException>(
                        lu.bssSection->sizeInBytes);
        if (lu.syscallBindingsSection)
            checkSectionSize<BindingsSectionTooBigException>(
                        calculateBindingsSize<
                            ThrowingBindingsSizeOverflowCheck<
                                BindingsSectionTooBigException> >(
                            lu.syscallBindingsSection->syscallBindings));
        if (lu.pdBindingsSection)
            checkSectionSize<BindingsSectionTooBigException>(
                        calculateBindingsSize<
                            ThrowingBindingsSizeOverflowCheck<
                                PdBindingsSectionTooBigException> >(
                            lu.pdBindingsSection->pdBindings));
        if (lu.debugSection)
            checkSectionSize<DebugSectionTooBigException>(
                        lu.debugSection->sizeInBytes);
    }

    {
        ExecutableCommonHeader header;
        header.init(static_cast<ExecutableCommonHeader::FileFormatVersionType>(
                        formatVersion));
        assert(header.isValid());
        if (!(os << header))
            return os;
    }

    {
        ExecutableHeader0x0 header0x0;
        header0x0.init(static_cast<ExecutableHeader0x0::NumLinkingUnitsSize>(
                           linkingUnits.size() - 1u),
                       static_cast<ExecutableHeader0x0::ActiveLinkingUnitIndex>(
                           activeLinkingUnit));
        assert(header0x0.isValid());
        if (!(os << header0x0))
            return os;
    }

    for (auto const & lu : linkingUnits) {
        {
            ExecutableLinkingUnitHeader0x0 luHeader0x0;
            using NSS = ExecutableLinkingUnitHeader0x0::NumSectionsSize;
            luHeader0x0.init(static_cast<NSS>(lu.numberOfSections() - 1u));
            if (!(os << luHeader0x0))
                return os;
        }

        static_assert(std::is_pod<ExecutableSectionHeader0x0>::value, "");
        if (lu.textSection) {
            auto const & instructions = lu.textSection->instructions;
            auto numInstructions = instructions.size();
            if (!serializeSectionHeader(
                    os,
                    ExecutableSectionHeader0x0::SectionType::Text,
                    numInstructions))
                return os;

            static constexpr auto const instructionsPerStreamSize =
                    std::numeric_limits<std::streamsize>::max()
                    / sizeof(SharemindCodeBlock);
            static_assert(instructionsPerStreamSize > 0u, "");
            auto writePtr = instructions.data();
            while (numInstructions > instructionsPerStreamSize) {
                auto const toWrite =
                        instructionsPerStreamSize * sizeof(SharemindCodeBlock);
                if (!os.write(reinterpret_cast<char const *>(writePtr),
                              static_cast<std::streamsize>(toWrite)))
                    return os;
                writePtr += instructionsPerStreamSize;
                numInstructions -= instructionsPerStreamSize;
            }
            auto const toWrite = numInstructions * sizeof(SharemindCodeBlock);
            if (!os.write(reinterpret_cast<char const *>(writePtr),
                          static_cast<std::streamsize>(toWrite)))
                return os;
        }

        using SectionType = ExecutableSectionHeader0x0::SectionType;

        if (lu.roDataSection
            && !serializeRegularSection(
                    os,
                    SectionType::RoData,
                    lu.roDataSection->data.get(),
                    lu.roDataSection->sizeInBytes))
            return os;

        if (lu.rwDataSection
            && !serializeRegularSection(
                    os,
                    SectionType::Data,
                    lu.rwDataSection->data.get(),
                    lu.rwDataSection->sizeInBytes))
            return os;

        if (lu.bssSection
            && !serializeSectionHeader(os,
                                       SectionType::Bss,
                                       lu.bssSection->sizeInBytes))
            return os;

        if (lu.syscallBindingsSection
            && !serializeBindingsSection(
                    os,
                    SectionType::Bind,
                    lu.syscallBindingsSection->syscallBindings))
            return os;

        if (lu.pdBindingsSection
            && !serializeBindingsSection(os,
                                         SectionType::PdBind,
                                         lu.pdBindingsSection->pdBindings))
            return os;

        if (lu.debugSection
            && !serializeRegularSection(
                    os,
                    ExecutableSectionHeader0x0::SectionType::Debug,
                    lu.debugSection->data.get(),
                    lu.debugSection->sizeInBytes))
            return os;
    }
    return os;
}

} /* namespace sharemind { */
