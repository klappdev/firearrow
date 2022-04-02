/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022 https://github.com/klappdev
 *
 * Permission is hereby  granted, free of charge, to any  person obtaining a copy
 * of this software and associated  documentation files (the "Software"), to deal
 * in the Software  without restriction, including without  limitation the rights
 * to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
 * copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
 * IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
 * FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
 * AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
 * LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BacktraceFrame.hpp"

#include "../util/strings/StringUtil.hpp"

namespace kl::backtrace {
    BacktraceFrame::BacktraceFrame()
        : absoluteAddress(0)
        , offset(0) {
    }

    BacktraceFrame::BacktraceFrame(const std::string& sourceFile, const std::string& sourceFunction,
                                   const uintptr_t absoluteAddress, const uintptr_t offset)
        : sourceFile(sourceFile)
        , sourceFunction(sourceFunction)
        , absoluteAddress(absoluteAddress)
        , offset(offset) {
    }

    BacktraceFrame::operator bool() const noexcept {
        return sourceFile.empty() && sourceFunction.empty() && absoluteAddress == 0 && offset == 0;
    }

    bool BacktraceFrame::operator==(const BacktraceFrame& other) const {
        return sourceFile == other.sourceFile && sourceFunction == other.sourceFunction &&
               absoluteAddress == other.absoluteAddress && offset == other.offset;
    }

    std::string toString(const BacktraceFrame& frame) {
        using util::strings::format;

        const auto& [fileName, functionName, address, offset] = frame;

        if (fileName.empty()) {
            return format("[at %p]", address);
        } else if (!functionName.empty()) {
            return format("[at %s:%p (%s+0x%x)]", fileName.c_str(), address, functionName.c_str(), (int32_t)offset);
        } else {
            return format("[at %s:%p]", fileName.c_str(), address);
        }
    }
}
