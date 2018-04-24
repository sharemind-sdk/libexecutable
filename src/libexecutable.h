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

#ifndef SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H
#define SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H

#include <array>
#include <cstdint>
#include <cstring>
#include <istream>
#include <sharemind/EndianMacros.h>


namespace sharemind {
class ExecutableCommonHeader;
}

std::istream & operator>>(std::istream &, sharemind::ExecutableCommonHeader &);

namespace sharemind {

class ExecutableCommonHeader {

    friend std::istream & ::operator>>(std::istream &,
                                       ExecutableCommonHeader &);

public: /* Types: */

    using FileFormatVersionType = std::uint16_t;

public: /* Methods: */

    void init(FileFormatVersionType version) noexcept;

    bool isValid() const noexcept;

    void serializeTo(void * buffer) const noexcept
    { std::memcpy(buffer, this, sizeof(*this)); }

    bool deserializeFrom(void const * data) noexcept
            __attribute__ ((nonnull(2), warn_unused_result));

    FileFormatVersionType fileFormatVersion() const noexcept
    { return littleEndianToHost(m_fileFormatVersion); }

    void setFileFormatVersion(FileFormatVersionType version) noexcept
    { m_fileFormatVersion = hostToLittleEndian(version); }

private: /* Fields: */

    std::array<char, 32u> m_magic;
    std::uint64_t m_byteOrderVerification;
    FileFormatVersionType m_fileFormatVersion;
    std::array<char,
               8u - ((sizeof(m_magic)
                      + sizeof(m_byteOrderVerification)
                      + sizeof(m_fileFormatVersion)) % 8u)>
            m_zeroPadding;

};

} /* namespace sharemind { */

#endif /* SHAREMIND_LIBEXECUTABLE_LIBEXECUTABLE_H */
