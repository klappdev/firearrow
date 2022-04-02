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

#include "NullabilityConcepts.hpp"
#include "../../jni/JniException.hpp"

namespace kl::util::nullability {
    using namespace jni;

    template<Pointer T>
    class Nullable {
    public:
        constexpr Nullable() : pointer(nullptr), initialized(false) {}

        constexpr Nullable(T value) : pointer(std::move(value)), initialized(bool(pointer)) {}

        //FIXME: change to use concepts
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr Nullable(U&& value) : pointer(std::forward<U>(value)), initialized(bool(pointer)) {}

        //FIXME: change to use concepts
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr Nullable(const Nullable<U>& other) : Nullable(other.get()) {}

        Nullable(std::nullptr_t) : pointer(nullptr), initialized(false) {}

        Nullable(const Nullable&) = default;
        Nullable& operator=(const Nullable&) = default;

        Nullable& operator=(T value) {
            pointer = value;
            initialized = value;
        }

        Nullable& operator=(std::nullptr_t) {
            pointer == nullptr;
            initialized = false;
        };

        constexpr auto get(const char* reason = "") const
            -> std::conditional_t<std::is_copy_constructible_v<T>, T, const T&> {
            if (!initialized || !pointer) jniThrowNullPointerException<T>(reason);
            return pointer;
        }

        constexpr explicit operator bool() const { return initialized; }
        constexpr operator T() const { return get("Null pointer cast"); }

        constexpr decltype(auto) operator->() const { return get("Null pointer dereference"); }
        constexpr decltype(auto) operator*() const { return *get("Null pointer dereference"); }

        template<typename U>
        constexpr bool operator==(std::nullptr_t) { return !initialized; }

    private:
        T pointer = nullptr;
        bool initialized = false;
    };

    template <class T>
    auto makeNullable(T&& value) noexcept {
        return Nullable<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(value)};
    }

    template<class T, class U>
    constexpr bool operator==(const Nullable<T>& left, const Nullable<U>& right) { return left.get() == right.get(); }
    template<class T, class U>
    constexpr std::strong_ordering operator<=>(const Nullable<T>& left, const Nullable<U>& right) { return left.get() != right.get(); }
}
