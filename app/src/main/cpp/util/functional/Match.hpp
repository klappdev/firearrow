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

#include "Result.hpp"

namespace kl::util::functional {

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


    template<typename ...Vs>
    class match {
    public:
        constexpr explicit match(const std::variant<Vs...>& data) : data(data) {}
        constexpr explicit match(std::variant<Vs...>&& data) : data(std::move(data)) {}

        template<class... Ts>
        constexpr decltype(auto) operator()(Ts&&... branches) {
            return std::visit(overloaded<Ts...>{std::forward<Ts>(branches)...}, data);
        }

    private:
        std::variant<Vs...> data;
    };

    template<typename V, typename E>
    class match<Result<V, E>> {
    public:
        constexpr explicit match(const Result<V, E>& data) : data(data) {}
        constexpr explicit match(Result<V, E>&& data) : data(std::move(data)) {}

        template<class... Ts>
        constexpr decltype(auto) operator()(Ts&&... branches) {
            static_assert(sizeof...(Ts) == 2, "Result<T,E> must have only two branches");

            return std::visit(overloaded<Ts...>{std::forward<Ts>(branches)...}, data.get());
        }
#if 0
        template<class VB, class EB>
        constexpr Result<V, E> operator()(VB valueBranch, EB errorBranch) {
            if (data.hasValue()) {
                return valueBranch(data.value());
            } else {
                return errorBranch(data.error());
            }
        }
#endif
    private:
        Result<V, E> data;
    };

    template<typename V, typename E>
    explicit match(const Result<V, E>&) -> match<Result<V, E>>;

    template<typename V, typename E>
    explicit match(Result<V, E>&&) -> match<Result<V, E>>;
}
