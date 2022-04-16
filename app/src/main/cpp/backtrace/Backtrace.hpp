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
#pragma once

#include <string>
#include <vector>

#include "BacktraceFrame.hpp"
#include "BacktraceState.hpp"

namespace kl::backtrace {

    class Backtrace final {
    public:
        using Iterator = std::vector<BacktraceFrame>::const_iterator;

        Backtrace();
        ~Backtrace() = default;

        Backtrace(const Backtrace&) = default;
        Backtrace(Backtrace&&) noexcept = default;

        Backtrace& operator=(const Backtrace&) = default;
        Backtrace& operator=(Backtrace&&) noexcept = default;

        static Backtrace current(size_t skip = BACKTRACE_SKIP_FRAMES, size_t maxDepth = BACKTRACE_MAX_FRAMES);

        [[nodiscard]] bool empty() const noexcept;
        size_t size() const noexcept;
        size_t maxSize() const noexcept;

        const BacktraceFrame& operator[](size_t index) const;

        Iterator begin() const noexcept;
        Iterator end() const noexcept;

        bool operator==(const Backtrace& other) const;

    private:
        std::string demangle(const char* name);
        bool isSharedLibrary(const char* name);

    private:
        size_t countFrames;
        size_t maxCountFrames;
        std::vector<BacktraceFrame> frames;
    };

    std::string toString(const Backtrace& backtrace);
}
