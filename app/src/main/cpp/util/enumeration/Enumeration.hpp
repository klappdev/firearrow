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

#include <array>
#include <string>
#include <optional>

#include <util/arrays/ArrayUtils.hpp>

namespace kl::util::enumeration {

    template<typename E>
    concept Enumerable = std::is_enum_v<E>;

    template<Enumerable E, std::size_t N>
    class Enumeration {
    public:
        constexpr Enumeration(const std::initializer_list<std::pair<E, const char*>> list)
            : data(arrays::makeArray<std::pair<E, const char*>, N>(list)) {}

        ~Enumeration() = default;

        [[nodiscard]] constexpr std::size_t count() const { return data.size(); }
        constexpr auto ordinal(E value) const { return static_cast<std::underlying_type_t<E>>(value); }

        constexpr const char* name(E enumerator) const {
            for (const auto& [key, value] : data) {
                if (key == enumerator) {
                    return value;
                }
            }

            return "<unknown>";
        }

        constexpr std::array<const char*, N> names() const {
            std::array<const char*, N> result;

            for (std::size_t i = 0; i < data.size(); i++) {
                result[i] = data[i].second;
            }

            return result;
        }

        constexpr std::optional<E> value(const char* name) const {
            for (const auto& [key, value] : data) {
                if (std::strcmp(value, name) == 0) {
                    return key;
                }
            }

            return std::nullopt;
        }

        constexpr std::array<E, N> values() const {
            std::array<E, N> result;

            for (std::size_t i = 0; i < data.size(); i++) {
                result[i] = data[i].first;
            }

            return result;
        }

    private:
        std::array<std::pair<E, const char*>, N> data;
    };
}
