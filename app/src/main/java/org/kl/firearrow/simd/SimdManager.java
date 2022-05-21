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
package org.kl.firearrow.simd;

import java.util.BitSet;

public final class SimdManager {

    private static final int[] ARRAY1 = {
        11, 22, 33, 44, 55, 66, 77, 88, 99, 88, 77, 66, 55, 44, 33, 22, 11
    };

    private static final int[] ARRAY2 = {
        22, 33, 44, 55, 66, 77, 88, 99, 11, 99, 88, 77, 66, 55, 44, 33, 22
    };

    private SimdManager() throws IllegalAccessException {
        throw new IllegalAccessException("Can't create instance");
    }

    public static native boolean isSupported(SimdAbi abi);

    public static native <T extends Number> SimdResult<T> sumArrays(T[] left, T[] right);

    public static native <T extends Number> SimdResult<T> sumArrays(T[] left, T[] right, SimdAbi abi);

    public static native <T extends Number> SimdResult<T> sumArrays(T[] left, T[] right, BitSet mask, SimdAbi abi);

    public static String javaAddTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    public static String cppAddTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    public static String javaStreamAddTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    public static String cppSimdAddTwoArrayNumbers() {
        return "Not implemented yet\n";
    }
}
