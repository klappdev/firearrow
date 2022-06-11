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

#include <util/nullability/NonNull.hpp>
#include <util/nullability/Nullable.hpp>

namespace kl::test {
    using kl::util::nullability::NonNull;
    using kl::util::nullability::Nullable;
    using kl::util::nullability::makeNonNull;
    using kl::util::nullability::makeNullable;

    TEST(NullabilityTest, constructNonNullFromLocalVariableTest) {
        int number = 1;
        NonNull<int*> ptr = &number;

        EXPECT_TRUE(*ptr == number);
    }

    TEST(NullabilityTest, constructNullableFromLocalVariableTest) {
        int number = 2;
        Nullable<int*> ptr = &number;

        EXPECT_TRUE(*ptr == number);
    }

    TEST(NullabilityTest, makeNonNullFromLocalVariableTest) {
        long number = 3;
        auto ptr = makeNonNull(&number);

        EXPECT_TRUE(*ptr == number);
    }

    TEST(NullabilityTest, makeNullableFromLocalVariableTest) {
        long number = 4L;
        auto ptr = makeNullable(&number);

        EXPECT_TRUE(*ptr == number);
    }

    TEST(NullabilityTest, makeNonNullFromRawPointerTest) {
        float number = 5.0f;
        float* rawPtr = &number;
        auto ptr = makeNonNull(rawPtr);

        EXPECT_TRUE(*ptr == number);
    }

    TEST(NullabilityTest, makeNullableFromRawPointerTest) {
        float number = 6.0f;
        float* rawPtr = &number;
        auto ptr = makeNullable(rawPtr);

        EXPECT_TRUE(*ptr == number);
    }
}

