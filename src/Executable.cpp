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
#include <new>
#include <sharemind/Concat.h>
#include <sharemind/GlobalDeleter.h>
#include <sharemind/IntegralComparisons.h>
#include <sharemind/ThrowNested.h>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include "libexecutable.h"
#include "libexecutable_0x0.h"


namespace sharemind {
namespace {

template <
        typename T,
        bool checkIsNeeded = (std::numeric_limits<T>::max()
                              >= std::numeric_limits<std::size_t>::max())
>
struct ValidNumberOfInstructionsCheck {
    static void check(T) noexcept {}
};

template <typename T>
struct ValidNumberOfInstructionsCheck<T, true> {
    static void check(T t) noexcept {
        if (t >= std::numeric_limits<std::size_t>::max())
            throw std::bad_array_new_length();
    }
};

template <typename T>
inline void checkValidNumberOfInstructions(T t)
{ return ValidNumberOfInstructionsCheck<T>::check(t); }

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

template <typename ... ExceptionGenerators>
std::istream & istreamSetFailure(std::istream & is,
                                 ExceptionGenerators ... exceptionGenerators)
{
    try {
        is.setstate(std::ios_base::failbit);
    } catch (std::ios_base::failure const & e) {
        throwNested(exceptionGenerators()..., e);
    }
    return is;
}

template <typename ValueToRead, typename ... ExceptionGenerators>
std::istream & istreamReadValue(std::istream & is,
                                ValueToRead & value,
                                ExceptionGenerators ... exceptionGenerators)
{
    try {
        return is >> value;
    } catch (std::ios_base::failure const & e) {
        throwNested(exceptionGenerators()..., e);
    }
}

template <typename ReadFailedException, typename ValueToRead>
std::istream & istreamReadValue(std::istream & is, ValueToRead & value)
{ return istreamReadValue(is, value, []() { return ReadFailedException(); }); }

template <typename SizeType, typename ... ExceptionGenerators>
std::istream & istreamReadRawData(std::istream & is,
                                  void * buffer,
                                  SizeType size,
                                  ExceptionGenerators ... exceptionGenerators)
{
    static constexpr auto const maxRead =
            std::numeric_limits<std::streamsize>::max();
    auto buf = static_cast<char *>(buffer);
    try {
        while (integralGreater(size, maxRead)) {
            if (!is.read(buf, maxRead))
                return is;
            size -= maxRead;
            buf += maxRead;
        }
        if (!is.read(buf, static_cast<std::streamsize>(size)))
            return is;
    } catch (std::ios_base::failure const & e) {
        throwNested(exceptionGenerators()..., e);
    }
    return is;
}

class LimitedBufferFilter: public std::streambuf {

public: /* Methods: */

    LimitedBufferFilter(LimitedBufferFilter &&) = delete;
    LimitedBufferFilter(LimitedBufferFilter const &) = delete;

    LimitedBufferFilter(std::istream & srcStream, std::size_t size)
        : m_srcStream(srcStream)
        , m_sizeLeft(size)
    {}

protected: /* Methods: */

    int_type underflow() override {
        if (gptr() < egptr())
            return traits_type::to_int_type(m_buf);
        if (m_sizeLeft <= 0u)
            return traits_type::eof();
        auto r(m_srcStream.get());
        if (r == traits_type::eof())
            return r;
        --m_sizeLeft;
        m_buf = traits_type::to_char_type(std::move(r));
        this->setg(&m_buf, &m_buf, &m_buf + 1u);
        return r;
    }

private: /* Fields: */

    std::istream & m_srcStream;
    std::size_t m_sizeLeft;
    char m_buf;

};

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
SHAREMIND_DEFINE_EXCEPTION_NOINLINE(Exception,
                                    Executable::,
                                    DeserializationException);
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToDeserializeFileHeaderException,
        "Failed to deserialize Sharemind executable file header!");
SHAREMIND_DEFINE_EXCEPTION_CONST_MSG_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToDeserializeFileHeader0x0Exception,
        "Failed to deserialize Sharemind executable file header specific to "
        "format version 0!");
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToDeserializeLinkingUnitHeader0x0Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToDeserializeSectionHeader0x0Exception);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleTextSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleRoDataSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleRwDataSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleBssSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleSyscallBindSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultiplePdBindSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        MultipleDebugSectionsInLinkingUnitException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadTextSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadRoDataSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadRwDataSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadBindSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadPdBindSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadDebugSectionDataException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        FailedToReadZeroPaddingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        InvalidZeroPaddingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        DuplicateSyscallBindingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        DuplicatePdBindingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        EmptySyscallBindingException);
SHAREMIND_DEFINE_EXCEPTION_CONST_STDSTRING_NOINLINE(
        DeserializationException,
        Executable::,
        EmptyPdBindingException);


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



Executable::TextSection::TextSection()
        noexcept(std::is_nothrow_default_constructible<Container>::value)
        = default;

Executable::TextSection::TextSection(TextSection &&)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
        = default;

Executable::TextSection::TextSection(TextSection const &) = default;

Executable::TextSection::TextSection(Container instructions_)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
    : instructions(std::move(instructions_))
{}

Executable::TextSection & Executable::TextSection::operator=(TextSection &&)
        noexcept(std::is_nothrow_move_assignable<Container>::value) = default;

Executable::TextSection & Executable::TextSection::operator=(
        TextSection const &) = default;



Executable::SyscallBindingsSection::SyscallBindingsSection()
        noexcept(std::is_nothrow_default_constructible<Container>::value)
        = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(
        SyscallBindingsSection &&)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
        = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(
        SyscallBindingsSection const &) = default;

Executable::SyscallBindingsSection::SyscallBindingsSection(Container bindings)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
    : syscallBindings(std::move(bindings))
{}

Executable::SyscallBindingsSection &
Executable::SyscallBindingsSection::operator=(SyscallBindingsSection &&)
        noexcept(std::is_nothrow_move_assignable<Container>::value) = default;

Executable::SyscallBindingsSection &
Executable::SyscallBindingsSection::operator=(SyscallBindingsSection const &)
        = default;



Executable::PdBindingsSection::PdBindingsSection()
        noexcept(std::is_nothrow_default_constructible<Container>::value)
        = default;

Executable::PdBindingsSection::PdBindingsSection(PdBindingsSection &&)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
        = default;

Executable::PdBindingsSection::PdBindingsSection(PdBindingsSection const &)
        = default;

Executable::PdBindingsSection::PdBindingsSection(Container pdBindings_)
        noexcept(std::is_nothrow_move_constructible<Container>::value)
    : pdBindings(std::move(pdBindings_))
{}

Executable::PdBindingsSection & Executable::PdBindingsSection::operator=(
        PdBindingsSection &&)
        noexcept(std::is_nothrow_move_assignable<Container>::value) = default;

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

} // namespace sharemind

std::ostream & operator<<(std::ostream & os, sharemind::Executable const & ex) {
    using namespace sharemind;
    using E = Executable;

    if (ex.fileFormatVersion != 0x0)
        throw Executable::FormatVersionNotSupportedException(
                concat("Sharemind Executable file format version ",
                       ex.fileFormatVersion,
                       " not supported for serialization!"));

    if (ex.linkingUnits.empty())
        throw E::NoLinkingUnitsDefinedException();
    if (ex.linkingUnits.size() - 1u
        > std::numeric_limits<ExecutableHeader0x0::NumLinkingUnitsSize>::max())
        throw E::TooManyLinkingUnitsDefinedException();
    if (ex.activeLinkingUnitIndex >= ex.linkingUnits.size())
        throw E::InvalidActiveLinkingUnitException();

    for (auto const & lu : ex.linkingUnits) {
        auto const numberOfSections = lu.numberOfSections();
        if (!numberOfSections)
            throw E::NoSectionsDefinedInLinkingUnitException();
        using NSS = ExecutableLinkingUnitHeader0x0::NumSectionsSize;
        if (numberOfSections - 1u > std::numeric_limits<NSS>::max())
            throw E::TooManySectionsDefinedInLinkingUnitException();
        if (lu.textSection)
            checkSectionSize<E::TextSectionTooBigException>(
                        lu.textSection->instructions.size());
        if (lu.roDataSection)
            checkSectionSize<E::RoDataSectionTooBigException>(
                        lu.roDataSection->sizeInBytes);
        if (lu.rwDataSection)
            checkSectionSize<E::RwDataSectionTooBigException>(
                        lu.rwDataSection->sizeInBytes);
        if (lu.bssSection)
            checkSectionSize<E::BssSectionTooBigException>(
                        lu.bssSection->sizeInBytes);
        if (lu.syscallBindingsSection)
            checkSectionSize<E::BindingsSectionTooBigException>(
                        calculateBindingsSize<
                            ThrowingBindingsSizeOverflowCheck<
                                E::BindingsSectionTooBigException> >(
                            lu.syscallBindingsSection->syscallBindings));
        if (lu.pdBindingsSection)
            checkSectionSize<E::BindingsSectionTooBigException>(
                        calculateBindingsSize<
                            ThrowingBindingsSizeOverflowCheck<
                                E::PdBindingsSectionTooBigException> >(
                            lu.pdBindingsSection->pdBindings));
        if (lu.debugSection)
            checkSectionSize<E::DebugSectionTooBigException>(
                        lu.debugSection->sizeInBytes);
    }

    {
        ExecutableCommonHeader header;
        header.init(static_cast<ExecutableCommonHeader::FileFormatVersionType>(
                        0x0));
        assert(header.isValid());
        if (!(os << header))
            return os;
    }

    {
        ExecutableHeader0x0 header0x0;
        header0x0.init(static_cast<ExecutableHeader0x0::NumLinkingUnitsSize>(
                           ex.linkingUnits.size() - 1u),
                       static_cast<ExecutableHeader0x0::ActiveLinkingUnitIndex>(
                           ex.activeLinkingUnitIndex));
        assert(header0x0.isValid());
        if (!(os << header0x0))
            return os;
    }

    for (auto const & lu : ex.linkingUnits) {
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

std::istream & operator>>(std::istream & is, sharemind::Executable & ex) {
    using namespace sharemind;
    using E = Executable;

    /* Reset output variable, set fileFormatVersion and activeLinkingUnitIndex
       to -1 to signal that the respective information has not yet been
       successfully read and parsed: */
    ex.fileFormatVersion = static_cast<std::size_t>(-1);
    ex.linkingUnits.clear();
    ex.activeLinkingUnitIndex = static_cast<std::size_t>(-1);

    ExecutableCommonHeader exeHeader;
    istreamReadValue<E::FailedToDeserializeFileHeaderException>(is, exeHeader);

    {
        auto const version(exeHeader.fileFormatVersion());
        static_assert(
                std::numeric_limits<decltype(version)>::max()
                <= std::numeric_limits<decltype(ex.fileFormatVersion)>::max(),
                "");
        ex.fileFormatVersion = version;
        if (version > 0u)
            return istreamSetFailure(
                    is,
                    [version]() {
                        return E::FormatVersionNotSupportedException(
                            concat("Sharemind Executable file format "
                                   "version ", version,
                                   " not supported for deserialization!"));
                    });
    }

    ExecutableHeader0x0 exeHeader0x0;
    istreamReadValue<E::FailedToDeserializeFileHeader0x0Exception>(
                is,
                exeHeader0x0);

    ex.activeLinkingUnitIndex = exeHeader0x0.activeLinkingUnitIndex();

    static std::size_t const extraPadding[8] =
            { 0u, 7u, 6u, 5u, 4u, 3u, 2u, 1u };
    char extraPaddingBuffer[8u];

    auto lusLeftMinusOne = exeHeader0x0.numberOfLinkingUnitsMinusOne();

    static_assert(std::numeric_limits<decltype(lusLeftMinusOne)>::max()
                  < std::numeric_limits<std::size_t>::max(), "");
    auto & linkingUnits = ex.linkingUnits;
    linkingUnits.reserve(static_cast<std::size_t>(lusLeftMinusOne) + 1u);

    std::size_t luIndex = 0u;
    for (;; --lusLeftMinusOne, ++luIndex) {
        ExecutableLinkingUnitHeader0x0 luHeader0x0;
        istreamReadValue(
                is,
                luHeader0x0,
                [luIndex]() {
                    return E::FailedToDeserializeLinkingUnitHeader0x0Exception(
                        concat("Failed to deserialize Sharemind executable "
                               "file linking unit header specific to "
                               "format version 0 for linking unit ",
                               luIndex, "!"));
                });

        #if __cplusplus >= 201703L
        auto & lu = linkingUnits.emplace_back();
        #else
        linkingUnits.emplace_back();
        auto & lu = linkingUnits.back();
        #endif

        auto sectionsLeftMinusOne = luHeader0x0.numberOfSectionsMinusOne();
        static_assert(std::numeric_limits<decltype(sectionsLeftMinusOne)>::max()
                      < std::numeric_limits<std::size_t>::max(), "");
        std::size_t sectionIndex = 0u;

        for (;; --sectionsLeftMinusOne, ++sectionIndex) {
            ExecutableSectionHeader0x0 sectionHeader0x0;
            istreamReadValue(
                    is,
                    sectionHeader0x0,
                    [luIndex, sectionIndex]() {
                        return E::FailedToDeserializeSectionHeader0x0Exception(
                            concat("Failed to deserialize Sharemind executable "
                                   "file linking unit header specific to "
                                   "format version 0 for linking unit ",
                                   luIndex, " and section ", sectionIndex,
                                   "!"));
                    });

            auto const sectionType = sectionHeader0x0.type();
            assert(sectionType
                   != ExecutableSectionHeader0x0::SectionType::Invalid);

            auto const sectionSize = sectionHeader0x0.size();
            static_assert(std::numeric_limits<decltype(sectionSize)>::max()
                          <= std::numeric_limits<std::size_t>::max(), "");

#define READ_AND_CHECK_ZERO_PADDING \
    do { \
        auto const paddingSize(extraPadding[sectionSize % 8]); \
        assert(sizeof(extraPaddingBuffer) >= paddingSize); \
        static_assert(std::numeric_limits<std::streamsize>::max() >= 8, \
                      ""); \
        if (!istreamReadRawData( \
                    is, \
                    extraPaddingBuffer, \
                    static_cast<std::streamsize>(paddingSize), \
                    [luIndex, sectionIndex]() { \
                        return E::FailedToReadZeroPaddingException( \
                            concat("Failed to read zero padding after " \
                                   "linking unit ", luIndex, ", section ", \
                                   sectionIndex, '!')); \
                    })) \
            return is; \
        for (unsigned i = 0u; i < paddingSize; ++i) \
            if (extraPaddingBuffer[i] != '\0') \
                return istreamSetFailure( \
                    is, \
                    [luIndex, sectionIndex]() { \
                        return E::InvalidZeroPaddingException( \
                            concat("Non-zero padding found after linking " \
                                   "unit ", luIndex, ", section ", \
                                   sectionIndex, '!')); \
                    }); \
    } while (false)
#define CHECK_DUPLICATE_SECTION(sName,eName,edesc) \
    if (lu.sName ## Section) { \
        return istreamSetFailure( \
            is, \
            [luIndex]() { \
                return E::Multiple ## eName ## SectionsInLinkingUnitException( \
                    concat("Multiple " edesc " sections defined in " \
                           "linking unit ", luIndex, '!')); \
            }); \
    } else (void) 0
#define INIT_DATASECTION(sName,eName,edesc) \
    do { \
        CHECK_DUPLICATE_SECTION(sName, eName, edesc); \
        auto newSection(std::make_shared<E::DataSection>()); \
        if (sectionSize <= 0u) \
            break; \
        newSection->data = \
                std::shared_ptr<void>(::operator new(sectionSize), \
                                      GlobalDeleter()); \
        newSection->sizeInBytes = sectionSize; \
        if (!istreamReadRawData( \
                is, \
                newSection->data.get(), \
                sectionSize, \
                [luIndex]() { \
                    return E::FailedToRead ## eName ## SectionDataException( \
                        concat("Failed to read contents of " edesc \
                               " section in linking unit ", luIndex, '!'));\
                })) \
            return is; \
        lu.sName ## Section = std::move(newSection); \
        READ_AND_CHECK_ZERO_PADDING; \
    } while(false)
#define INIT_BINDSECTION(sName,eName,edesc) \
    do { \
        CHECK_DUPLICATE_SECTION(sName, eName, edesc); \
        if (sectionSize <= 0) \
            break; \
        std::unordered_set<std::string> bsSet; \
        LimitedBufferFilter limitedBuffer(is, sectionSize); \
        std::istream is2(&limitedBuffer); \
        std::string bindName; \
        auto newSection(std::make_shared<E::eName ## ingsSection>()); \
        std::vector<std::string> & bs = newSection->sName; \
        while (std::getline(is2, bindName, '\0')) { \
            if (bindName.empty()) \
                return istreamSetFailure( \
                    is, \
                    [luIndex, sectionIndex]() { \
                        return E::Empty ## eName ## ingException( \
                            concat("Invalid empty binding found in " edesc \
                                   " section in linking unit ", luIndex, \
                                   ", section ", sectionIndex, '!')); \
                    }); \
            if (bsSet.find(bindName) != bsSet.cend()) \
                return istreamSetFailure( \
                    is, \
                    [luIndex, sectionIndex, &bindName]() { \
                        return E::Duplicate ## eName ## ingException( \
                            concat("Duplicate binding for \"", bindName, \
                                   "\" found in " edesc " section in linking " \
                                   "unit ", luIndex, ", section ", \
                                   sectionIndex, '!')); \
                    }); \
            bs.emplace_back(bindName); \
            bsSet.emplace(std::move(bindName)); \
        } \
        lu.sName ## Section = std::move(newSection); \
        READ_AND_CHECK_ZERO_PADDING; \
    } while(false)

            switch (sectionType) {
            case ExecutableSectionHeader0x0::SectionType::Text:
                CHECK_DUPLICATE_SECTION(text, Text, "text");

                lu.textSection = std::make_shared<E::TextSection>();
                if (sectionSize <= 0u)
                    break;
                {
                    auto & instructions = lu.textSection->instructions;
                    checkValidNumberOfInstructions(sectionSize);
                    instructions.reserve(
                                static_cast<std::size_t>(sectionSize) + 1u);
                    instructions.resize(sectionSize);
                    if (!istreamReadRawData(
                            is,
                            instructions.data(),
                            sectionSize * sizeof(SharemindCodeBlock),
                            [luIndex]() {
                                return E::FailedToReadTextSectionDataException(
                                    concat("Failed to read contents of "
                                           "text section in linking unit ",
                                           luIndex, '!'));
                            }))
                        return is;
                }
                break;
            case ExecutableSectionHeader0x0::SectionType::RoData:
                INIT_DATASECTION(roData, RoData, "read-only data");
                break;
            case ExecutableSectionHeader0x0::SectionType::Data:
                INIT_DATASECTION(rwData, RwData, "read-write data");
                break;
            case ExecutableSectionHeader0x0::SectionType::Bss:
                CHECK_DUPLICATE_SECTION(bss, Bss, "BSS");
                lu.bssSection = std::make_shared<E::BssSection>(sectionSize);
                break;
            case ExecutableSectionHeader0x0::SectionType::Bind:
                INIT_BINDSECTION(syscallBindings,
                                 SyscallBind,
                                 "system call bindings");
                break;
            case ExecutableSectionHeader0x0::SectionType::PdBind:
                INIT_BINDSECTION(pdBindings,
                                 PdBind,
                                 "protection domain bindings");
                break;
            default:
                assert(sectionType ==
                       ExecutableSectionHeader0x0::SectionType::Debug);
                INIT_DATASECTION(debug, Debug, "debug");
                break;
            }
            if (!sectionsLeftMinusOne)
                break;
        } // Loop over sections in linking unit

        if (!lusLeftMinusOne)
            break;
    } // Loop over linking units

    return is;
}
