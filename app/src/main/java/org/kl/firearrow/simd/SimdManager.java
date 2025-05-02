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
package org.kl.firearrow.simd;

import org.kl.firearrow.time.StopWatch;

import java.util.Arrays;

import dalvik.annotation.optimization.CriticalNative;
import dalvik.annotation.optimization.FastNative;

public final class SimdManager {

    private static final int[] ARRAY1 = {
        11, 22, 33, 44, 55, 66, 77, 88, 99, 88, 77, 66, 55, 44, 33, 22
    };

    private static final int[] ARRAY2 = {
        22, 33, 44, 55, 66, 77, 88, 99, 11, 99, 88, 77, 66, 55, 44, 33
    };

    private final StopWatch stopWatch;

    public SimdManager() {
        this.stopWatch = new StopWatch();
    }

    @CriticalNative
    private static native boolean nativeIsSimdSupported();

    private static native SimdResult nativeSimdAddArrays(int[] leftArray, int[] rightArray) throws SimdException;

    @FastNative
    private static native int[] nativeFastSimdAddArrays(int[] leftArray, int[] rightArray) throws SimdException;

    private static native SimdResult nativeSimdSubArrays(int[] leftArray, int[] rightArray) throws SimdException;

    @FastNative
    private static native int[] nativeFastSimdSubArrays(int[] leftArray, int[] rightArray) throws SimdException;

    public String javaAdditionTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java addition two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        int[] resultArray = new int[ARRAY1.length];

        for (int i = 0; i < ARRAY1.length; i++) {
            resultArray[i] = ARRAY1[i] + ARRAY2[i];
        }

        builder.append(" Result array: ").append(Arrays.toString(resultArray)).append("\n");

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppSimdAdditionTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ simd addition two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        try {
            final SimdResult simdResult = nativeSimdAddArrays(ARRAY1, ARRAY2);

            builder.append(" Result array: ").append(Arrays.toString(simdResult.value())).append("\n");

            builder.append("> Simd abi is supported: ").append(simdResult.isAbiSupported()).append("\n");

            builder.append("> C++ execution time: ").append(simdResult.duration()).append(" ms\n");
        } catch (SimdException e) {
            builder.append("> Simd addition two integer arrays, exception")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppFastSimdAdditionTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ fast simd addition two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        try {
            final int[] simdResult = nativeFastSimdAddArrays(ARRAY1, ARRAY2);

            builder.append(" Result array: ").append(Arrays.toString(simdResult)).append("\n");
        } catch (SimdException e) {
            builder.append("> Simd addition two integer arrays, exception")
                    .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String javaSubtractTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java subtract two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        int[] resultArray = new int[ARRAY1.length];

        for (int i = 0; i < ARRAY1.length; i++) {
            resultArray[i] = ARRAY2[i] - ARRAY1[i];
        }

        builder.append(" Result array: ").append(Arrays.toString(resultArray)).append("\n");

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppSimdSubtractTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ simd subtract two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        try {
            final SimdResult simdResult = nativeSimdSubArrays(ARRAY2, ARRAY1);

            builder.append(" Result array: ").append(Arrays.toString(simdResult.value())).append("\n");

            builder.append("> Simd abi is supported: ").append(simdResult.isAbiSupported()).append("\n");

            builder.append("> C++ execution time: ").append(simdResult.duration()).append(" ms\n");
        } catch (SimdException e) {
            builder.append("> Simd addition two integer arrays, exception")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppFastSimdSubtractTwoIntegerArrays() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ fast simd subtract two integer arrays\n");

        builder.append(" First array: ").append(Arrays.toString(ARRAY2)).append("\n");
        builder.append(" Second array: ").append(Arrays.toString(ARRAY1)).append("\n");

        try {
            final int[] simdResult = nativeFastSimdSubArrays(ARRAY2, ARRAY1);

            builder.append(" Result array: ").append(Arrays.toString(simdResult)).append("\n");
        } catch (SimdException e) {
            builder.append("> Simd addition two integer arrays, exception")
                    .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppCheckSimdAbiIsSupported() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ check simd abi is supported\n");

        final boolean simdAbiSupported = nativeIsSimdSupported();

        builder.append(" Simd Neon abi 128 is supported: ").append(simdAbiSupported ? "true" : "false").append("\n");

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }
}
