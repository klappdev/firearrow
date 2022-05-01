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

#include <gtest/gtest.h>

#include <util/enumeration/Enumeration.hpp>

namespace kl::test {
    using kl::util::enumeration::Enumeration;

    enum class Season {
        SPRING, SUMMER, AUTUMN, WINTER
    };

    static inline constexpr Enumeration<Season, 4> SEASONS = {
        {Season::SPRING, "Spring"},
        {Season::SUMMER, "Summer"},
        {Season::AUTUMN, "Autumn"},
        {Season::WINTER, "Winter"},
    };

    TEST(EnumerationTest, createEnumerationTest) {
        EXPECT_EQ(SEASONS.count(), 4);
        EXPECT_EQ(SEASONS.ordinal(Season::SPRING), 0);
    }

    TEST(EnumerationTest, compareEnumerationNamesTest) {
        constexpr std::array<const char*, 4> names = {"Spring", "Summer", "Autumn", "Winter"};
        EXPECT_STREQ(SEASONS.name(Season::SUMMER), "Summer");
        EXPECT_EQ(SEASONS.names(), names);
    }

    TEST(EnumerationTest, compareEnumerationValuesTest) {
        constexpr std::array<Season, 4> values = {Season::SPRING, Season::SUMMER, Season::AUTUMN, Season::WINTER};
        EXPECT_EQ(*SEASONS.value("Winter"), Season::WINTER);
        EXPECT_EQ(SEASONS.values(), values);
    }
}