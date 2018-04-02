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

#include "libexecutable.h"

#include <cassert>
#include <type_traits>
#include <utility>


namespace sharemind {

static_assert(sizeof(ExecutableCommonHeader) == 32u + 8u + 2u + 6u, "");
static_assert(sizeof(ExecutableCommonHeader) % 8u == 0u, "");
static_assert(std::is_pod<ExecutableCommonHeader>::value, "");

namespace {

std::array<char, 32u> const commonHeaderMagic{"Sharemind Executable"};

} // anonymous namespace

void ExecutableCommonHeader::init(FileFormatVersionType version) noexcept {
    m_magic = commonHeaderMagic;
    m_byteOrderVerification =
            hostToLittleEndian(
                decltype(m_byteOrderVerification)(0x0123456789abcdef));
    m_fileFormatVersion = hostToLittleEndian(version);
    std::memset(m_zeroPadding.data(), '\0', m_zeroPadding.size());
}

bool ExecutableCommonHeader::isValid() const noexcept {
    if (m_magic != commonHeaderMagic)
        return false;
    if (littleEndianToHost(m_byteOrderVerification) != 0x0123456789abcdef)
        return false;
    for (auto const c : m_zeroPadding)
        if (c != '\0')
            return false;
    return true;
}

bool ExecutableCommonHeader::deserializeFrom(void const * data) noexcept {
    assert(data);
    ExecutableCommonHeader buf;
    std::memcpy(&buf, data, sizeof(buf));
    if (!buf.isValid())
        return false;
    (*this) = std::move(buf);
    return true;
}

} // namespace sharemind {
