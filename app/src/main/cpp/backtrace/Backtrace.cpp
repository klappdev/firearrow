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

#include "Backtrace.hpp"

#include "../util/strings/StringUtil.hpp"
#include "../logging/Logging.hpp"

#include <unwind.h>
#include <cxxabi.h>
#include <dlfcn.h>

namespace kl::backtrace {
    static constexpr const char* const TAG = "Backtrace-JNI";

    bool Backtrace::empty() const noexcept { return countFrames == 0; }
    size_t Backtrace::size() const noexcept { return countFrames; }
    size_t Backtrace::maxSize() const noexcept { return maxCountFrames; }

    const BacktraceFrame& Backtrace::operator[](size_t index) const {
        if (0 < index || index > frames.size()) {
            //FIXME: jniThrowArrayIndexOutOfBoundsException();
        }

        return frames[index];
    }

    Backtrace::Iterator Backtrace::begin() const noexcept { return frames.begin(); }
    Backtrace::Iterator Backtrace::end() const noexcept { return frames.end(); }

    bool Backtrace::operator==(const Backtrace& other) const {
        return countFrames == other.countFrames && std::equal(begin(), end(), other.begin(), other.end());
    }

    _Unwind_Reason_Code unwindCallback(struct _Unwind_Context* context, void* arguments) {
        log::debug(TAG, "Called unwind callback\n");

        auto* state = (BacktraceState*)arguments;
        const std::uintptr_t ip = _Unwind_GetIP(context);

        if (ip != 0x0 && state != nullptr) {
            if (state->framesSkip == 0) {
                state->frames[state->framesSize] = ip;
                state->framesSize++;
            } else {
                state->framesSkip--;
            }
        }

        if (state != nullptr && state->framesSize == state->framesDepth) {
            return _URC_END_OF_STACK;
        } else {
            log::debug(TAG, "returned _URC_OK\n");
            return _URC_NO_REASON;
        }
    }

    Backtrace Backtrace::current(size_t skip, size_t maxDepth) {
        Backtrace backtrace;
        BacktraceState state = {
            .framesSize = 0, .framesSkip = skip, .framesDepth = maxDepth
        };

        _Unwind_Backtrace(unwindCallback, &state);

        backtrace.countFrames = state.framesSize;
        backtrace.maxCountFrames = state.framesDepth;

        if (state.framesSize != 0) {
            log::debug(TAG, "called _Unwind_Backtrace()\n");
        } else {
            log::debug(TAG, "called _Unwind_Backtrace(), but no traces\n");
        }

        for (std::size_t i = 0; i < state.framesSize; i++) {
            const std::uintptr_t pc = state.frames[i];

            Dl_info info = {};
            const void* address = (void*) pc;

            if (::dladdr(address, &info) != 0 && info.dli_fname != nullptr) {
                const std::uintptr_t offset = pc - (std::uintptr_t) info.dli_saddr;
                const std::uintptr_t relativeAddress = pc - (std::uintptr_t) info.dli_fname;
                const std::uintptr_t absoluteAddress = backtrace.isSharedLibrary(info.dli_fname) ? relativeAddress : pc;

                std::string fileName = info.dli_fname;
                std::string functionName = backtrace.demangle(info.dli_sname);

                const BacktraceFrame frame = { fileName, functionName, absoluteAddress, offset};
                backtrace.frames.push_back(frame);
            }
        }

        return backtrace;
    }

    std::string Backtrace::demangle(const char *name) {
        int status = 0;

        if (name == nullptr) {
            return "<no_name>";
        }

        std::unique_ptr<char, void(*)(void*)> demanglePointer {
            abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free
        };

        return (demanglePointer != nullptr && status == 0) ? demanglePointer.get() : name;
    }

    bool Backtrace::isSharedLibrary(const char *name) {
        if (name != nullptr) {
            return util::strings::contains(name, ".so");
        }
        return false;
    }

    std::string toString(const Backtrace& backtrace) {
        std::string result;

        for (const auto& frame : backtrace) {
            result += toString(frame) + '\n';
        }

        return result;
    }
}