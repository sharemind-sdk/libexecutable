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

#include "libexecutable_0x0.h"

#include <cassert>
#include <sharemind/IntegralComparisons.h>
#include <type_traits>
#include <utility>


namespace sharemind {

static_assert(sizeof(ExecutableHeader0x0) == 1u + 1u + 6u, "");
static_assert(sizeof(ExecutableHeader0x0) % 8u == 0u, "");
static_assert(std::is_pod<ExecutableHeader0x0>::value, "");
static_assert(sizeof(ExecutableLinkingUnitHeader0x0) == 32u + 1u + 7u, "");
static_assert(sizeof(ExecutableLinkingUnitHeader0x0) % 8u == 0u, "");
static_assert(std::is_pod<ExecutableLinkingUnitHeader0x0>::value, "");
static_assert(sizeof(ExecutableSectionHeader0x0) == 32u + 4u + 4u, "");
static_assert(sizeof(ExecutableSectionHeader0x0) % 8u == 0u, "");
static_assert(std::is_pod<ExecutableSectionHeader0x0>::value, "");

namespace {

using HeaderTypeHeader = std::array<char, 32u>;

HeaderTypeHeader const luMagic{"Linking Unit"};
using ST = ExecutableSectionHeader0x0::SectionType;
std::array<HeaderTypeHeader,
           static_cast<std::underlying_type<ST>::type>(ST::Count)> const
        sMagic{HeaderTypeHeader{"TEXT"},
               HeaderTypeHeader{"RODATA"},
               HeaderTypeHeader{"DATA"},
               HeaderTypeHeader{"BSS"},
               HeaderTypeHeader{"BIND"},
               HeaderTypeHeader{"PDBIND"},
               HeaderTypeHeader{"DEBUG"}};

} // anonymous namespace

void ExecutableHeader0x0::init(NumLinkingUnitsSize numberOfUnitsMinusOne,
                               ActiveLinkingUnitIndex activeLinkingUnit)
        noexcept
{
    m_numberOfLinkingUnitsMinusOne = numberOfUnitsMinusOne;
    m_activeLinkingUnitIndex = activeLinkingUnit;
    std::memset(m_zeroPadding.data(), '\0', m_zeroPadding.size());
}

bool ExecutableHeader0x0::isValid() const noexcept {
    if (m_activeLinkingUnitIndex > m_numberOfLinkingUnitsMinusOne)
        return false;
    for (auto const c : m_zeroPadding)
        if (c != '\0')
            return false;
    return true;
}

bool ExecutableHeader0x0::deserializeFrom(void const * data) noexcept {
    assert(data);

    ExecutableHeader0x0 buf;
    std::memcpy(&buf, data, sizeof(buf));
    if (!buf.isValid())
        return false;
    (*this) = std::move(buf);
    return true;
}

void ExecutableLinkingUnitHeader0x0::init(std::uint8_t sectionsMinusOne)
        noexcept
{
    m_type = luMagic;
    m_sectionsMinusOne = sectionsMinusOne;
    std::memset(&m_zeroPadding, '\0', m_zeroPadding.size());
}

bool ExecutableLinkingUnitHeader0x0::isValid() const noexcept {
    using STU = std::underlying_type<ST>::type;
    if (m_type != luMagic
        || integralGreater(m_sectionsMinusOne, static_cast<STU>(ST::Count)))
        return false;
    for (auto const c : m_zeroPadding)
        if (c != '\0')
            return false;
    return true;
}

bool ExecutableLinkingUnitHeader0x0::deserializeFrom(void const * const data)
        noexcept
{
    assert(data);

    ExecutableLinkingUnitHeader0x0 buf;
    std::memcpy(&buf, data, sizeof(buf));
    if (!buf.isValid())
        return false;
    (*this) = std::move(buf);
    return true;
}

void ExecutableSectionHeader0x0::init(SectionType type, SizeType length)
        noexcept
{
    if (type == SectionType::Invalid) {
        m_type = decltype(m_type){"<INVALID>"};
    } else {
        m_type = sMagic[static_cast<decltype(sMagic)::size_type>(type)];
    }
    m_length = hostToLittleEndian(length);
    std::memset(m_zeroPadding.data(), '\0', m_zeroPadding.size());
}

bool ExecutableSectionHeader0x0::isValid() const noexcept {
    if (type() == SectionType::Invalid)
        return false;
    for (auto const c : m_zeroPadding)
        if (c != '\0')
            return false;
    return true;
}

bool ExecutableSectionHeader0x0::deserializeFrom(void const * from) noexcept {
    assert(from);

    ExecutableSectionHeader0x0 buf;
    std::memcpy(&buf, from, sizeof(buf));
    if (!buf.isValid())
        return false;
    (*this) = std::move(buf);
    return true;
}

ExecutableSectionHeader0x0::SectionType ExecutableSectionHeader0x0::type()
        const noexcept
{
#define MATCH_TYPE(e) \
    if (m_type \
        == sMagic[static_cast<decltype(sMagic)::size_type>(SectionType::e)]) \
        return SectionType::e
    MATCH_TYPE(Text);
    MATCH_TYPE(RoData);
    MATCH_TYPE(Data);
    MATCH_TYPE(Bss);
    MATCH_TYPE(Bind);
    MATCH_TYPE(PdBind);
    MATCH_TYPE(Debug);
#undef MATCH_TYPE
    return SectionType::Invalid;
}

} // namespace sharemind {
