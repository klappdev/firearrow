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
#include <future>

template<typename R, typename... Args>
struct std::experimental::coroutine_traits<std::future<R>, Args...> {
    struct promise_type {
    public:
        auto get_return_object() /*customisable*/ { return offer.get_future(); }

        std::experimental::suspend_never initial_suspend() const noexcept /*customisable*/ { return {}; }
        std::experimental::suspend_never final_suspend() const noexcept /*customisable*/ { return {}; }

        void set_exception(std::exception_ptr error) {
            offer.set_exception(std::move(error));
        }

        void unhandled_exception() /*customisable*/ {
            std::current_exception();
        }

        template <typename U>
        void return_value(U&& value) /*customisable*/ {
            offer.set_value(std::forward<U>(value));
        }
    private:
        std::promise<R> offer;
    };
};

template<typename... Args>
struct std::experimental::coroutine_traits<std::future<void>, Args...> {
    struct promise_type {
    public:
        auto get_return_object() /*customisable*/ { return offer.get_future(); }

        std::experimental::suspend_never initial_suspend() const noexcept /*customisable*/ { return {}; }
        std::experimental::suspend_never final_suspend() const noexcept /*customisable*/ { return {}; }

        void set_exception(std::exception_ptr e) {
            offer.set_exception(std::move(e));
        }

        void unhandled_exception() /*customisable*/ {
            std::current_exception();
        }

        void return_void() /*customisable*/ {
            offer.set_value();
        }
    private:
        std::promise<void> offer;
    };
};
