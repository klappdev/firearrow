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

import static org.kl.firearrow.util.ContextUtils.*;

import android.view.View;

import org.kl.firearrow.fs.FileException;
import org.kl.firearrow.fs.FileManager;
import org.kl.firearrow.fs.OverwriteMode;

public final class ChooseFeatureListener implements View.OnClickListener {
    private final long featureId;

    public ChooseFeatureListener(long featureId) {
        this.featureId = featureId;
    }

    @Override
    public void onClick(View view) {
        final String result;

        switch ((int) featureId) {
            case 1: result = javaDeleteFile(); break;
            case 2: result = cppEraseFile(OverwriteMode.SIMPLE_MODE); break;
            case 3: result = cppEraseFile(OverwriteMode.DOE_MODE); break;
            case 4: result = cppEraseFile(OverwriteMode.OPENBSD_MODE); break;
            case 5: result = cppEraseFile(OverwriteMode.RCMP_MODE); break;
            case 6: result = cppEraseFile(OverwriteMode.DOD_MODE); break;
            case 7: result = javaDeleteDirectory(); break;
            case 8: result = cppEraseDirectory(); break;
            case 9: result = javaEraseDirectoryRecursive(); break;
            case 10: result = cppEraseDirectoryRecursive(); break;

            case 11: result = javaThreadOperation(); break;
            case 12: result = cppCoroutineAwaitableOperation(); break;
            case 13: result = cppCoroutineRunnableOperation(); break;
            case 14: result = cppCoroutineCallableOperation(); break;
            case 15: result = javaStreamGenerateNumbers(); break;
            case 16: result = cppGeneratorYieldNumbers(); break;
            case 17: result = javaStreamGenerateSequence(); break;
            case 18: result = cppGeneratorYieldSequence(); break;

            case 19: result = javaSummaryTwoArrayNumbers(); break;
            case 20: result = cppSummaryTwoArrayNumbers(); break;
            case 21: result = javaStreamSummaryTwoArrayNumbers(); break;
            case 22: result = cppSimdSummaryTwoArrayNumbers(); break;
            default: result = "Unknown operation!!!"; break;
        }

        toast(view.getContext(), "Finished operation " + result);
    }

    private String javaDeleteFile() {
        return "Not implemented yet";
    }

    private String cppEraseFile(OverwriteMode mode) {
        final String tmpFile = "/path/file.txt";

        try {
            final long duration = FileManager.eraseFile(tmpFile, mode);

            return "Erase file " + tmpFile + "successfully with duration => " + duration + " ms";
        } catch (FileException e) {
            return "Erase file " + tmpFile + "with exception" + e.getMessage();
        }
    }

    private String javaDeleteDirectory() {
        return "Not implemented yet";
    }

    private String cppEraseDirectory() {
        return "Not implemented yet";
    }

    private String javaEraseDirectoryRecursive() {
        return "Not implemented yet";
    }

    private String cppEraseDirectoryRecursive() {
        return "Not implemented yet";
    }

    private String javaThreadOperation() {
        return "Not implemented yet";
    }

    private String cppCoroutineAwaitableOperation() {
        return "Not implemented yet";
    }

    private String cppCoroutineRunnableOperation() {
        return "Not implemented yet";
    }

    private String cppCoroutineCallableOperation() {
        return "Not implemented yet";
    }

    private String javaStreamGenerateNumbers() {
        return "Not implemented yet";
    }

    private String cppGeneratorYieldNumbers() {
        return "Not implemented yet";
    }

    private String javaStreamGenerateSequence() {
        return "Not implemented yet";
    }

    private String cppGeneratorYieldSequence() {
        return "Not implemented yet";
    }

    private String javaSummaryTwoArrayNumbers() {
        return "Not implemented yet";
    }

    private String cppSummaryTwoArrayNumbers() {
        return "Not implemented yet";
    }

    private String javaStreamSummaryTwoArrayNumbers() {
        return "Not implemented yet";
    }

    private String cppSimdSummaryTwoArrayNumbers() {
        return "Not implemented yet";
    }
}
