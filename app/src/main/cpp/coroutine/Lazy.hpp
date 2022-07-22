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

#include <util/property/Setter.hpp>
#include <util/nullability/Nullable.hpp>
#include <util/error/Result.hpp>

namespace kl::coroutine {
    using namespace kl::util::property;
    using namespace kl::util::nullability;
    using namespace kl::util::error;

    template<typename T>
    class Lazy;

    class LazyPromiseBase {
    public:
        explicit LazyPromiseBase() noexcept : continuation(continuation_) {}

        constexpr auto initial_suspend() const noexcept /*customisable*/ { return std::experimental::suspend_always(); }
        constexpr auto final_suspend() const noexcept /*customisable*/ { return Awaitable(); }

        Setter<std::experimental::coroutine_handle<>&> continuation;

    private:
        struct Awaitable final {
            bool await_ready() const noexcept /*customisable*/ { return false; }
            void await_resume() noexcept /*customisable*/ {}

            template<typename Promise>
            std::experimental::coroutine_handle<>
            await_suspend(std::experimental::coroutine_handle<Promise> handle) noexcept /*customisable*/ {
                return handle.promise().continuation_;
            }
        };

        std::experimental::coroutine_handle<> continuation_;
    };

    template<typename T>
    class LazyPromise final : public LazyPromiseBase {
    public:
        LazyPromise() noexcept = default;
        ~LazyPromise() = default;

        Lazy<T> get_return_object() noexcept /*customisable*/;

        void unhandled_exception() noexcept /*customisable*/ {
            data = std::current_exception();
        }

        template<typename V>
        void return_value(V&& value) noexcept /*customisable*/ {
            data = std::forward<V>(value);
        }

        const T& result() const {
            if (data.hasError()) {
                std::rethrow_exception(data.error());
            }

            return data.value();
        }
    private:
        Result<T, std::exception_ptr> data;
    };

    template<>
    class LazyPromise<void> final : public LazyPromiseBase {
    public:
        LazyPromise() noexcept = default;

        Lazy<void> get_return_object() noexcept /*customisable*/;

        void unhandled_exception() noexcept /*customisable*/ {
            error = std::current_exception();
        }

        void return_value() noexcept /*customisable*/ {}

        void result() const {
            if (error) {
                std::rethrow_exception(error);
            }
        }

    private:
        std::exception_ptr error;
    };

    template<typename T>
    class LazyPromise<T&> final : public LazyPromiseBase {
    public:
        LazyPromise() noexcept = default;
        ~LazyPromise() = default;

        Lazy<T&> get_return_object() noexcept /*customisable*/;

        void unhandled_exception() noexcept /*customisable*/ {
            error = std::current_exception();
        }

        template<typename V>
        void return_value(V& other) noexcept /*customisable*/ {
            value = std::addressof(other);
        }

        T& result() const {
            if (error) {
                std::rethrow_exception(error);
            }

            return *value;
        }
    private:
        Nullable<T*> value;
        std::exception_ptr error;
    };

    template<typename T = void>
    class [[nodiscard]] Lazy {
    public:
        /*customisable*/
        using promise_type = LazyPromise<T>;

        Lazy() noexcept : continuation(nullptr) {}
        explicit Lazy(std::experimental::coroutine_handle<promise_type> continuation)
            : continuation(continuation) {
        }

        ~Lazy() {
            if (continuation) {
                continuation.destroy();
            }
        }

        Lazy(const Lazy&) = delete;
        Lazy& operator=(const Lazy&) = delete;

        Lazy(Lazy&& other) noexcept : continuation(other.continuation) {
            other.continuation = nullptr;
        }

        Lazy& operator=(Lazy&& other) {
            if (std::addressof(other) != this) {
                if (continuation) {
                    continuation.destroy();
                }
            }

            continuation = other.continuation;
            other.continuation = nullptr;

            return *this;
        }

        [[nodiscard]] bool isReady() const noexcept {
            return !continuation || continuation.done();
        }

        auto operator co_await() const noexcept {
            struct Awaitable final {

                explicit Awaitable(std::experimental::coroutine_handle<promise_type> continuation)
                    : continuation(continuation) {
                }

                bool await_ready() const noexcept /*customisable*/ {
                    return !continuation || continuation.done();
                }

                decltype(auto) await_resume() /*customisable*/ {
                    return continuation.promise().result();
                }

                std::experimental::coroutine_handle<>
                await_suspend(std::experimental::coroutine_handle<> handle) noexcept /*customisable*/ {
                    continuation.promise().continuation = handle;
                    return continuation;
                }

                std::experimental::coroutine_handle<promise_type> continuation;
            };

            return Awaitable(continuation);
        }

    private:
        std::experimental::coroutine_handle<promise_type> continuation;
    };

    template<typename T>
    Lazy<T> LazyPromise<T>::get_return_object() noexcept {
        return Lazy<T>(std::experimental::coroutine_handle<LazyPromise>::from_promise(*this));
    }

    Lazy<void> LazyPromise<void>::get_return_object() noexcept {
        return Lazy<void>(std::experimental::coroutine_handle<LazyPromise>::from_promise(*this));
    }

    template<typename T>
    Lazy<T&> LazyPromise<T&>::get_return_object() noexcept {
        return Lazy<T&>(std::experimental::coroutine_handle<LazyPromise>::from_promise(*this));
    }
}
