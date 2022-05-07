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
package org.kl.firearrow.coroutine;

import java.util.Arrays;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.stream.Collectors;
import java.util.stream.IntStream;

public final class CoroutineManager {

    private CoroutineManager() throws IllegalAccessException {
        throw new IllegalAccessException("Can't create instance");
    }

    public static native Task<Void> await(Runnable runnable) throws CoroutineException;
    public static native <T> Task<T> await(Callable<T> caller) throws CoroutineException;

    public static native <T extends Number> Generator<T> yield(T initValue, int count) throws CoroutineException;
    public static native <T extends Number> Generator<T> yield(T initValue, int begin, int end) throws CoroutineException;

    public static String javaThreadRunnableOperation() {
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart Java thread sort array numbers\n");

        try {
            final String[] result = new String[1];
            final int[] numbers = new int[] { 1, 10, 2, 9, 3, 8, 4, 7, 5, 6 };

            result[0] = Arrays.stream(numbers)
                              .mapToObj(String::valueOf)
                              .collect(Collectors.joining(","));
            builder.append("> Sequence before: ").append("[").append(result[0]).append("]").append("\n");

            final ExecutorService executor = Executors.newSingleThreadExecutor();
            final Future<?> future = executor.submit(() -> {
                Arrays.sort(numbers);
                result[0] = Arrays.stream(numbers)
                                  .mapToObj(String::valueOf)
                                  .collect(Collectors.joining(","));
            });
            future.get();

            builder.append("> Sequence after: ").append("[").append(result[0]).append("]").append("\n");
        } catch (ExecutionException | InterruptedException e) {
            builder.append("> Thread exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> Java execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String cppCoroutineRunnableOperation() {
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart C++ coroutine sort array numbers\n");

        try {
            final String[] result = new String[1];
            final int[] numbers = new int[] { 1, 10, 2, 9, 3, 8, 4, 7, 5, 6 };

            result[0] = Arrays.stream(numbers)
                              .mapToObj(String::valueOf)
                              .collect(Collectors.joining(","));
            builder.append("> Sequence before: ").append("[").append(result[0]).append("]").append("\n");

            final Task<Void> task = CoroutineManager.await(() -> {
                Arrays.sort(numbers);
                result[0] = Arrays.stream(numbers)
                                  .mapToObj(String::valueOf)
                                  .collect(Collectors.joining(","));
            });

            if (!task.isFinished()) {
                builder.append("> Coroutine not finished\n");
            }

            builder.append("> Sequence after: ").append("[").append(result[0]).append("]").append("\n");
            builder.append("> C++ execution time: ").append(task.getDuration()).append(" ms\n");
        } catch (CoroutineException e) {
            builder.append("> Coroutine exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> JNI execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String javaThreadCallableOperation() {
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart Java thread binary search in array numbers\n");

        try {
            final int[] numbers = new int[] { 11, 20, 12, 19, 13, 18, 14, 17, 15, 16 };
            final int key = 13;

            String result = Arrays.stream(numbers)
                                  .mapToObj(String::valueOf)
                                  .collect(Collectors.joining(","));
            builder.append("> Sequence : ").append("[").append(result).append("]").append("\n");

            final ExecutorService executor = Executors.newSingleThreadExecutor();
            final Future<String> future = executor.submit(() -> {
                final int index = Arrays.binarySearch(numbers, key);
                return "value " + key + ", index " + index;
            });
            result = future.get();

            builder.append("> Searched ").append(result).append("\n");
        } catch (ExecutionException | InterruptedException e) {
            builder.append("> Thread exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> Java execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String cppCoroutineCallableOperation() {
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart C++ thread binary search in array numbers\n");

        try {
            final int[] numbers = new int[] { 11, 20, 12, 19, 13, 18, 14, 17, 15, 16 };
            final int key = 13;

            final String result = Arrays.stream(numbers)
                                        .mapToObj(String::valueOf)
                                        .collect(Collectors.joining(","));
            builder.append("> Sequence : ").append("[").append(result).append("]").append("\n");

            final Task<String> task = CoroutineManager.await(() -> {
                final int index = Arrays.binarySearch(numbers, key);
                return "value " + key + ", index " + index;
            });

            builder.append("> Searched ").append(task.getValue()).append("\n");
            builder.append("> C++ execution time: ").append(task.getDuration()).append(" ms\n");
        } catch (CoroutineException e) {
            builder.append("> Generator exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> JNI execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String javaStreamGenerateNumbers() {
        final int initValue = 1;
        final int countValues = 11;
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart Java stream generate numbers with init value ")
               .append(initValue).append(", count ").append(countValues).append("\n");

        final String result = IntStream.range(initValue, countValues)
                                       .boxed()
                                       .map(String::valueOf)
                                       .collect(Collectors.joining(","));
        builder.append("> Sequence: ").append("[").append(result).append("]").append("\n");

        final long endTime = System.currentTimeMillis();
        builder.append("> Java execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String cppGeneratorYieldNumbers() {
        final int initValue = 1;
        final int countValues = 10;
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart C++ generator yielding numbers with init value ")
               .append(initValue).append(", count ").append(countValues).append("\n");
        try {
            final var generator = CoroutineManager.yield(initValue, countValues);

            builder.append("> Sequence: [");

            for (int number : generator) {
                builder.append(number).append(",");
            }

            builder.append("]\n");
            builder.append("> C++ execution time: ").append(generator.getDuration()).append(" ms\n");
        } catch (CoroutineException e) {
            builder.append("> Generator exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> JNI execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String javaStreamGenerateSequence() {
        final int initValue = 1;
        final int startRange = 10;
        final int endRange = 20;
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart Java stream generate numbers with init value ")
               .append(initValue).append(", from ")
               .append(startRange).append(" to ").append(endRange).append("\n");

        final String result = IntStream.range(startRange, endRange)
                .map(i -> i + initValue)
                .boxed()
                .map(String::valueOf)
                .collect(Collectors.joining(","));

        builder.append("> Sequence: ").append("[").append(result).append("]").append("\n");

        final long endTime = System.currentTimeMillis();
        builder.append("> Java execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String cppGeneratorYieldSequence() {
        final int initValue = 1;
        final int startRange = 10;
        final int endRange = 20;
        final long beginTime = System.currentTimeMillis();
        final var builder = new StringBuilder();

        builder.append("\nStart C++ generator yielding numbers with init value ")
               .append(initValue).append(", from ")
               .append(startRange).append(" to ").append(endRange).append("\n");
        try {
            final var generator = CoroutineManager.yield(initValue, startRange, endRange);

            builder.append("> Sequence: [");

            for (int number : generator) {
                builder.append(number).append(",");
            }

            builder.append("]\n");
            builder.append("> C++ execution time: ").append(generator.getDuration()).append(" ms\n");
        } catch (CoroutineException e) {
            builder.append("> Generator exception: ").append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> JNI execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }
}
