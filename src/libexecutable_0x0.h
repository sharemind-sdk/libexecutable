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

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H

#include <array>
#include <cstdint>
#include <cstring>
#include <sharemind/EndianMacros.h>


namespace sharemind {

/*******************************************************************************
  Format 0x0 header.
*******************************************************************************/

class ExecutableHeader0x0 {

public: /* Types: */

    using NumLinkingUnitsSize = std::uint8_t;
    using ActiveLinkingUnitIndex = NumLinkingUnitsSize;

public: /* Methods: */

    void init(NumLinkingUnitsSize numberOfUnitsMinusOne,
              ActiveLinkingUnitIndex activeLinkingUnit) noexcept;

    bool isValid() const noexcept;

    void serializeTo(void * buffer) const noexcept
    { std::memcpy(buffer, this, sizeof(*this)); }

    bool deserializeFrom(void const * data) noexcept
        __attribute__ ((nonnull(2), warn_unused_result));

    NumLinkingUnitsSize numberOfLinkingUnitsMinusOne() const noexcept
    { return m_numberOfLinkingUnitsMinusOne; }

    void setNumberOfLinkingUnitsMinusOne(
            NumLinkingUnitsSize const numberOfLinkingUnitsMinusOne) noexcept
    { m_numberOfLinkingUnitsMinusOne = numberOfLinkingUnitsMinusOne; }

    void setActiveLinkingUnitIndex(
            ActiveLinkingUnitIndex const activeLinkingUnitIndex) noexcept
    { m_activeLinkingUnitIndex = activeLinkingUnitIndex; }

    ActiveLinkingUnitIndex activeLinkingUnitIndex() const noexcept
    { return m_activeLinkingUnitIndex; }

private: /* Fields: */

    NumLinkingUnitsSize m_numberOfLinkingUnitsMinusOne;
    ActiveLinkingUnitIndex m_activeLinkingUnitIndex;
    std::array<char,
               8u - ((sizeof(m_numberOfLinkingUnitsMinusOne)
                      + sizeof(m_activeLinkingUnitIndex)) % 8u)>
            m_zeroPadding;

};


/*******************************************************************************
  Format 0x0 unit header.
*******************************************************************************/

class ExecutableLinkingUnitHeader0x0 {

public: /* Types: */

    using NumSectionsSize = std::uint8_t;

public: /* Methods: */

    void init(NumSectionsSize sectionsMinusOne) noexcept;

    bool isValid() const noexcept;

    void serializeTo(void * buffer) const noexcept
    { std::memcpy(buffer, this, sizeof(*this)); }

    bool deserializeFrom(void const * data) noexcept
        __attribute__ ((nonnull(2), warn_unused_result));

    NumSectionsSize numberOfSectionsMinusOne() const noexcept
    { return m_sectionsMinusOne; }

    void setNumberOfSectionsMinusOne(NumSectionsSize const sectionsMinusOne)
            noexcept
    { m_sectionsMinusOne = sectionsMinusOne; }

private: /* Fields: */

    std::array<char, 32u> m_type;
    NumSectionsSize m_sectionsMinusOne;
    std::array<char, 8u - ((sizeof(m_type) + sizeof(m_sectionsMinusOne)) % 8u)>
            m_zeroPadding;

};


/*******************************************************************************
  Format 0x0 section header.
*******************************************************************************/

class ExecutableSectionHeader0x0 {

public: /* Types: */

    using SizeType = std::uint32_t;

    enum class SectionType {
        Invalid = -1,
        Text = 0,
        RoData = 1,
        Data = 2,
        Bss = 3,
        Bind = 4,
        PdBind = 5,
        Debug = 6,
        Count = 7
    };

public: /* Methods: */

    void init(SectionType type, SizeType length) noexcept;

    bool isValid() const noexcept;

    void serializeTo(void * buffer) const noexcept
    { std::memcpy(buffer, this, sizeof(*this)); }

    bool deserializeFrom(void const * data) noexcept
        __attribute__ ((nonnull(2), warn_unused_result));

    SectionType type() const noexcept;
    void setType(SectionType const type) noexcept;

    SizeType size() const noexcept { return littleEndianToHost(m_length); }

    void setSize(SizeType const size) noexcept
    { m_length = hostToLittleEndian(size); }

private: /* Fields: */

    std::array<char, 32u> m_type;
    SizeType m_length;
    std::array<char, 8u - ((sizeof(m_type) + sizeof(m_length)) % 8u)>
            m_zeroPadding;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_0x0_H */
