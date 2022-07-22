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

#include <util/nullability/Nullable.hpp>

namespace kl::coroutine {
    using namespace kl::util::nullability;

    template<typename T>
    class Generator;

    template<typename T>
    struct GeneratorPromise final {
        using Reference = std::conditional_t<std::is_reference_v<T>, T, T&>;
        using Pointer = Nullable<std::remove_reference_t<T>*>;

        GeneratorPromise() : value(nullptr) {}

        Generator<T> get_return_object() noexcept /*customisable*/;

        constexpr std::experimental::suspend_always initial_suspend() const noexcept /*customisable*/ { return {}; }
        constexpr std::experimental::suspend_always final_suspend() const noexcept /*customisable*/ { return {}; }

        template<typename U = T,
                 typename std::enable_if_t<!std::is_rvalue_reference_v<U>>>
        std::experimental::suspend_always yield_value(std::remove_reference_t<T>& data) noexcept /*customisable*/ {
            this->value = std::addressof(data);
            return {};
        }

        std::experimental::suspend_always yield_value(std::remove_reference_t<T>&& data) noexcept /*customisable*/ {
            this->value = std::addressof(data);
            return {};
        }

        void return_void() /*customisable*/ {}

        void unhandled_exception() /*customisable*/ {
            this->error = std::current_exception();
        }

        void rethrowIfError() {
            if (error) {
                std::rethrow_exception(std::move(error));
            }
        }

        Pointer value;
        std::exception_ptr error;
    };

    template<typename T>
    Generator<T> GeneratorPromise<T>::get_return_object() noexcept {
        using handle = std::experimental::coroutine_handle<GeneratorPromise<T>>;
        return Generator<T>{ handle::from_promise(*this) };
    }

    struct GeneratorSentinel final {};

    template<typename T>
    class GeneratorIterator final {
    public:
        GeneratorIterator() noexcept : continuation(nullptr) {}

        explicit GeneratorIterator(std::experimental::coroutine_handle<GeneratorPromise<T>> coroutine) noexcept
            : continuation(coroutine) {
        }

        friend bool operator==(const GeneratorIterator& self, GeneratorSentinel) noexcept {
            return !self.continuation || self.continuation.done();
        }

        friend bool operator==(GeneratorSentinel sentinel, const GeneratorIterator& self) noexcept {
            return (sentinel == self);
        }

        GeneratorIterator& operator++() {
            continuation.resume();

            if (continuation.done()) {
                continuation.promise().rethrowIfError();
            }

            return *this;
        }

        void operator++(int) { (void) operator++(); }

        typename GeneratorPromise<T>::Reference operator*() const noexcept {
            return static_cast<typename GeneratorPromise<T>::Reference>(*continuation.promise().value);
        }

        typename GeneratorPromise<T>::Pointer operator->() const noexcept {
            return std::addressof(operator*());
        }
    private:
        std::experimental::coroutine_handle<GeneratorPromise<T>> continuation;
    };

    template<typename T>
    class [[nodiscard]] Generator final {
    public:
        using promise_type = GeneratorPromise<T>;

        Generator() noexcept : continuation(nullptr) {}

        Generator(const Generator& other) = delete;
        Generator(Generator&& other) noexcept : continuation(other.continuation) {
            other.continuation = nullptr;
        }

        ~Generator() {
            if (continuation) {
                continuation.destroy();
            }
        }

        GeneratorIterator<T> begin() {
            if (continuation) {
                continuation.resume();

                if (continuation.done()) {
                    continuation.promise().rethrowIfError();
                }
            }

            return GeneratorIterator<T> { continuation };
        }

        GeneratorSentinel end() noexcept { return {}; }

        bool hasError() { return (bool) continuation.promise().error; }

    private:
        friend class GeneratorPromise<T>;

        explicit Generator(std::experimental::coroutine_handle<promise_type> coroutine) noexcept
            : continuation(coroutine) {
        }

        std::experimental::coroutine_handle<promise_type> continuation;
    };
}
