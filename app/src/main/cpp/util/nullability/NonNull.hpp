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
    class NonNull {
    public:
        constexpr NonNull(T value) requires NonNullPointer<T> : pointer(std::move(value)) {
            if (!pointer) jniThrowNullPointerException<T>("Construct from null pointer");
        }

        //FIXME: change to use concepts
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr NonNull(U&& value) : pointer(std::forward<U>(value)) {
            if (!pointer) jniThrowNullPointerException<T>("Construct from null pointer");
        }

        //FIXME: change to use concepts
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, T>>>
        constexpr NonNull(const NonNull<U>& other) : NonNull(other.get()) {}

        NonNull(const NonNull&) = default;
        NonNull& operator=(const NonNull&) = default;

        NonNull(std::nullptr_t) = delete;
        NonNull& operator=(std::nullptr_t) = delete;

        constexpr auto get(const char* reason = "") const
            -> std::conditional_t<std::is_copy_constructible_v<T>, T, const T&> {
            if (!pointer) jniThrowNullPointerException<T>(reason);
            return pointer;
        }

        constexpr operator T() const { return get("Null pointer cast"); }
        constexpr decltype(auto) operator->() const { return get("Null pointer dereference"); }
        constexpr decltype(auto) operator*() const { return *get("Null pointer dereference"); }

    private:
        T pointer = nullptr;
    };

    template <class T>
    auto makeNonNull(T&& value) noexcept {
        return NonNull<std::remove_cv_t<std::remove_reference_t<T>>>{std::forward<T>(value)};
    }

    template<class T, class U>
    constexpr bool operator==(const NonNull<T>& left, const NonNull<U>& right) { return left.get() == right.get(); }
    template<class T, class U>
    constexpr std::strong_ordering operator<=>(const NonNull<T>& left, const NonNull<U>& right) { return left.get() != right.get(); }
}