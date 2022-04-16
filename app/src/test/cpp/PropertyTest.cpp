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

#include <util/property/Property.hpp>
#include <util/property/Setter.hpp>
#include <util/property/Getter.hpp>

namespace kl::test {
    using kl::util::property::Property;
    using kl::util::property::Setter;
    using kl::util::property::Getter;

    static int testNumber = 5;
    static std::string testString("text");

    TEST(PropertyTest, constructGetterFromLvalueVariableTest) {

        Getter<int> property(testNumber);

        EXPECT_EQ(property, testNumber);
        int result = property;
        EXPECT_EQ(result, property);
    }

    TEST(PropertyTest, constructSetterFromLvalueVariableTest) {
        Setter<int> property(testNumber);

        EXPECT_EQ(property, testNumber);
        property = 10;
        EXPECT_EQ(property, 10);
    }

    TEST(PropertyTest, constructGetterFromRvalueVariableTest) {
        Getter<std::string> property("text");
        EXPECT_TRUE(property == testString);
    }

    TEST(PropertyTest, constructSetterFromRvalueVariableTest) {
        Setter<std::string> property("text");
        EXPECT_TRUE(property == testString);
        property = "new text";
        EXPECT_TRUE(property == std::string("new text"));
    }

    TEST(PropertyTest, constructGetterAsMemberClassTest) {
        struct Data {
            Getter<std::string> property{std::string("init data")};
        } data;

        std::string result = data.property;
        EXPECT_TRUE(result == std::string("init data"));
    }

    TEST(PropertyTest, constructSetterAsMemberClassTest) {
        struct Data {
            Setter<std::string> property{std::string("init data")};
        } data;

        data.property = "new data";
        EXPECT_TRUE(data.property == std::string("new data"));
    }

    TEST(PropertyTest, constructGetterReferenceAsMemberClassTest) {
        struct Data {
            std::string tmp;
            Getter<std::string&> property{tmp};
        } data;

        std::string tmpString = "ref data";
        data.tmp = tmpString;
        std::string result = data.property;
        EXPECT_TRUE(result == tmpString);
        EXPECT_TRUE(data.tmp == tmpString);
    }

    TEST(PropertyTest, constructSetterReferenceAsMemberClassTest) {
        struct Data {
            std::string tmp;
            Setter<std::string&> property{tmp};
        } data;

        std::string tmpString = "ref data";
        data.tmp = tmpString;
        EXPECT_TRUE(data.property == tmpString);
        EXPECT_TRUE(data.tmp == tmpString);
    }

    TEST(PropertyTest, constructGetterPointerAsMemberClassTest) {
        struct Data {
            std::string tmp;
            Getter<std::string*> property{&tmp};
        } data;

        std::string tmpString = "ptr data";
        data.tmp = tmpString;
        EXPECT_TRUE(data.property == &tmpString);
        EXPECT_TRUE(data.tmp == tmpString);
    }

    TEST(PropertyTest, constructSetterPointerAsMemberClassTest) {
        struct Data {
            std::string tmp;
            Setter<std::string*> property{&tmp};
        } data;

        std::string tmpString = "ptr data";
        data.tmp = tmpString;
        EXPECT_TRUE(data.property == &tmpString);
        EXPECT_TRUE(data.tmp == tmpString);
    }
}

