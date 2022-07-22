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

#include <experimental/coroutine>

#include <util/property/Getter.hpp>

namespace kl::coroutine {
    using namespace kl::util::property;

    template<typename T>
    class Task final {
    public:
        explicit Task(std::function<T()> callback)
            : value(value_)
            , finished(finished_)
            , callback(std::move(callback))
            , value_(nullptr)
            , finished_(false) {
        }

        ~Task() = default;

        bool await_ready() const /*customisable*/ { return false; }
        void await_resume() /*customisable*/ {}
        void await_suspend(std::experimental::coroutine_handle<> handler) /*customisable*/ {
            value_ = callback();

            if (!handler.done()) {
                finished_ = true;
                handler.resume();
            }
        }

        Getter<T&> value;
        Getter<bool&> finished;

    private:
        std::function<T()> callback;
        T value_;
        bool finished_;
    };

    template<>
    class Task<void> final {
    public:
        explicit Task(std::function<void()> callback)
            : finished(finished_)
            , callback(std::move(callback))
            , finished_(false) {
        }

        ~Task() = default;

        bool await_ready() const /*customisable*/ { return false; }
        void await_resume() /*customisable*/ {}
        void await_suspend(std::experimental::coroutine_handle<> handler) /*customisable*/ {
            callback();

            if (!handler.done()) {
                finished_ = true;
                handler.resume();
            }
        }

        Getter<bool&> finished;

    private:
        std::function<void()> callback;
        bool finished_;
    };
}