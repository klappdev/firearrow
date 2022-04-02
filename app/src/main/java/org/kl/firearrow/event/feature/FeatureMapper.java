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

package org.kl.firearrow.event.feature;

import android.os.Build;

import androidx.annotation.RequiresApi;

import org.kl.firearrow.fs.EraseException;
import org.kl.firearrow.fs.FileSystemManager;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.Supplier;

import javax.inject.Inject;

public final class FeatureMapper {
    @Inject
    public FeatureMapper() {
    }

    @RequiresApi(api = Build.VERSION_CODES.R)
    private final List<Supplier<String>> mappers = List.of(
        this::javaDeleteFile,
        this::cppEraseFileWithSimpleMode,
        this::cppEraseFileWithDoeMode,
        this::cppEraseFileWithOpenBsdMode,
        this::cppEraseFileWithRcmpMode,
        this::cppEraseFileWithDodMode,
        this::cppEraseFileWithGutmanMode,
        this::javaDeleteTwoFiles,
        this::cppEraseTwoFiles,
        this::javaDeleteDirectory,
        this::cppEraseDirectory,
        this::javaEraseDirectoryRecursive,
        this::cppEraseDirectoryRecursive,

        this::javaThreadExecuteOperation,
        this::cppThreadExecuteOperation,
        this::javaExecutorExecuteOperation,
        this::cppCoroutineTaskExecuteOperation,
        this::javaExecutorExecuteOperationWithResult,
        this::cppCoroutineTaskExecuteOperationWithResult,
        this::javaStreamGenerateNumbers,
        this::cppGeneratorTakeNumbers,
        this::javaStreamGenerateSequence,
        this::cppGeneratorTakeSequence,
        this::javaStreamGenerateOperationNumbers,
        this::cppGeneratorOperationNumbers,

        this::javaSummaryTwoArrayNumbers,
        this::cppSummaryTwoArrayNumbers,
        this::javaStreamSummaryTwoArrayNumbers,
        this::cppSimdSummaryTwoArrayNumbers
    );

    @RequiresApi(api = Build.VERSION_CODES.R)
    public Supplier<String> findById(int id) {
        return mappers.get(id);
    }

    private String javaDeleteFile() {
        return null;
    }

    private String cppEraseFileWithSimpleMode() {
        final String tmpFile = ""; //FIXME: create tmp file

        try {
            final boolean flag = FileSystemManager.eraseFile(tmpFile);

            return "Erase file " + tmpFile + (flag ? "successfully" : "not successfully");
        } catch (EraseException e) {
            return "Erase file " + tmpFile + "with exception" + e.getMessage();
        }
    }

    private String cppEraseFileWithDoeMode() {
        return null;
    }

    private String cppEraseFileWithOpenBsdMode() {
        return null;
    }

    private String cppEraseFileWithRcmpMode() {
        return null;
    }

    private String cppEraseFileWithDodMode() {
        return null;
    }

    private String cppEraseFileWithGutmanMode() {
        return null;
    }

    private String javaDeleteTwoFiles() {
        return null;
    }

    private String cppEraseTwoFiles() {
        return null;
    }

    private String javaDeleteDirectory() {
        return null;
    }

    private String cppEraseDirectory() {
        return null;
    }

    private String javaEraseDirectoryRecursive() {
        return null;
    }

    private String cppEraseDirectoryRecursive() {
        return null;
    }

    private String javaThreadExecuteOperation() {
        return null;
    }

    private String cppThreadExecuteOperation() {
        return null;
    }

    private String javaExecutorExecuteOperation() {
        return null;
    }

    private String cppCoroutineTaskExecuteOperation() {
        return null;
    }

    private String javaExecutorExecuteOperationWithResult() {
        return null;
    }

    private String cppCoroutineTaskExecuteOperationWithResult() {
        return null;
    }

    private String javaStreamGenerateNumbers() {
        return null;
    }

    private String cppGeneratorTakeNumbers() {
        return null;
    }

    private String javaStreamGenerateSequence() {
        return null;
    }

    private String cppGeneratorTakeSequence() {
        return null;
    }

    private String javaStreamGenerateOperationNumbers() {
        return null;
    }

    private String cppGeneratorOperationNumbers() {
        return null;
    }

    private String javaSummaryTwoArrayNumbers() {
        return null;
    }

    private String cppSummaryTwoArrayNumbers() {
        return null;
    }

    private String javaStreamSummaryTwoArrayNumbers() {
        return null;
    }

    private String cppSimdSummaryTwoArrayNumbers() {
        return null;
    }
}
