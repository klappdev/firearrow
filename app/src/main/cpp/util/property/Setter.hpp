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

#include <functional>

namespace kl::util::property {

    template<typename T>
    class Setter {
    public:
        constexpr explicit Setter(const T& value) : value(value) {}
        constexpr explicit Setter(T&& value) : value(std::forward<T>(value)) {}
        ~Setter() = default;

        constexpr Setter(const Setter&) = default;
        constexpr Setter(Setter&&) noexcept = default;

        constexpr Setter& operator=(const Setter&) = default;
        constexpr Setter& operator=(Setter&&) noexcept = default;

        constexpr Setter& operator=(const T& newValue) {
            this->value = newValue;
            return *this;
        }

        constexpr Setter& operator=(T&& newValue) {
            this->value = std::forward<T>(newValue);
            return *this;
        }

        constexpr bool operator==(const T& other) const { return value == other; }
        constexpr auto operator<=>(const T& other) const { return value <=> other; }
        constexpr auto operator<=>(const Setter<T>& other) const = default;

    private:
        T value;
    };

    template<typename T>
    class Setter<T&> {
    public:
        constexpr explicit Setter(T& value) : value(std::ref(value)) {}

        constexpr Setter& operator=(T& newValue) {
            value.get() = newValue;
            return *this;
        }

        constexpr bool operator==(T& other) const { return value.get() == other; }
        constexpr auto operator<=>(T& other) const { return value.get() <=> other; }

    private:
        std::reference_wrapper<T> value;
    };

    template<typename T>
    class Setter<T*> {
    public:
        constexpr explicit Setter(T* value) : value(std::ref(*value)) {}

        constexpr Setter& operator=(T* newValue) {
            value.get() = *newValue;
            return *this;
        }

        constexpr bool operator==(T* other) const { return value.get() == *other; }
        constexpr auto operator<=>(T* other) const { return value.get() <=> *other; }

    private:
        std::reference_wrapper<T> value;
    };
}