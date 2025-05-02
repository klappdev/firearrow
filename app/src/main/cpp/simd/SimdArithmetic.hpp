/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2025 https://github.com/klappdev
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

#include <vector>

#include <arm_neon.h>

namespace firearrow::simd {

    enum class SimdArithmeticType : std::uint8_t {
        SIMD_ADDITION = 0, SIMD_SUBTRACT = 1
    };

    void addNeonArray(int32_t leftArray[], int32_t rightArray[], int32_t targetArray[], size_t size) {
        //Android Neon only supports 128-bit SIMD operations
        constexpr std::size_t simdAbiStep = 4;

        for (size_t i = 0; i < size; i += simdAbiStep) {
            /* Load data into NEON register */
            int32x4_t simdLeftArray = vld1q_s32(&(leftArray[i]));
            int32x4_t simdRightArray = vld1q_s32(&(rightArray[i]));

            /* Perform the addition */
            int32x4_t simdTargetArray = vaddq_s32(simdLeftArray, simdRightArray);

            /* Store the result from NEON register */
            vst1q_s32(&(targetArray[i]), simdTargetArray);
        }
    }

    void subNeonArray(int32_t leftArray[], int32_t rightArray[], int32_t targetArray[], size_t size) {
        //Android Neon only supports 128-bit SIMD operations
        constexpr std::size_t simdAbiStep = 4;

        for (size_t i = 0; i < size; i += simdAbiStep) {
            /* Load data into NEON register */
            int32x4_t simdLeftArray = vld1q_s32(&(leftArray[i]));
            int32x4_t simdRightArray = vld1q_s32(&(rightArray[i]));

            /* Perform the subtract */
            int32x4_t simdTargetArray = vsubq_s32(simdLeftArray, simdRightArray);

            /* Store the result from NEON register */
            vst1q_s32(&(targetArray[i]), simdTargetArray);
        }
    }

/* C++ <experimental/simd> is currently not supported

    template<std::size_t N>
    std::vector<std::int32_t> sumSimdArray(const std::vector<std::int32_t>& leftArray,
                                           const std::vector<std::int32_t>& rightArray) {
        std::vector<std::int32_t> resultArray = {};

        constexpr std::size_t simdSize = (N / sizeof(std::int32_t));

        std::experimental::fixed_size_simd<std::int32_t, simdSize> leftVector;
        std::experimental::fixed_size_simd<std::int32_t, simdSize> rightVector;

        for (std::size_t i = 0; i < leftArray.size() / simdSize; ++i) {
            leftVector.copy_from(&leftArray[i * simdSize], std::experimental::element_aligned);
            rightVector.copy_from(&rightArray[i * simdSize], std::experimental::element_aligned);

            auto resultVector = leftVector + rightVector;
            resultVector.copy_to(&resultArray[i * simdSize], std::experimental::element_aligned);
        }

        return resultArray;
    }
*/
}


