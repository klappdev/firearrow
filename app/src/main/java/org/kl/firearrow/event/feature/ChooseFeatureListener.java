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

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.schedulers.Schedulers;

import org.kl.firearrow.coroutine.CoroutineManager;
import org.kl.firearrow.fs.FileManager;
import org.kl.firearrow.fs.OverwriteMode;
import org.kl.firearrow.ui.feature.FeatureActivity;

public final class ChooseFeatureListener implements View.OnClickListener {
    private final long featureId;
    private final FeatureActivity activity;

    public ChooseFeatureListener(long featureId, FeatureActivity activity) {
        this.featureId = featureId;
        this.activity = activity;
    }

    @Override
    public void onClick(View view) {
        activity.getDisposables().add(Observable.fromCallable(this::executeFeature)
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribe((String result) -> {
                activity.showExecutionFeature(result);

                toast(view.getContext(), "Finished operation !!!");
            }));
    }

    private String executeFeature() {
        final String result;

        switch ((int) featureId) {
            case 1: result = FileManager.javaDeleteFile(); break;
            case 2: result = FileManager.cppEraseFile(OverwriteMode.SIMPLE_MODE); break;
            case 3: result = FileManager.cppEraseFile(OverwriteMode.DOE_MODE); break;
            case 4: result = FileManager.cppEraseFile(OverwriteMode.OPENBSD_MODE); break;
            case 5: result = FileManager.cppEraseFile(OverwriteMode.RCMP_MODE); break;
            case 6: result = FileManager.cppEraseFile(OverwriteMode.DOD_MODE); break;
            case 7: result = FileManager.javaDeleteDirectory(); break;
            case 8: result = FileManager.cppEraseDirectory(); break;
            case 9: result = FileManager.javaEraseDirectoryRecursive(); break;
            case 10: result = FileManager.cppEraseDirectoryRecursive(); break;

            case 11: result = CoroutineManager.javaThreadRunnableOperation(); break;
            case 12: result = CoroutineManager.cppCoroutineRunnableOperation(); break;
            case 13: result = CoroutineManager.javaThreadCallableOperation(); break;
            case 14: result = CoroutineManager.cppCoroutineCallableOperation(); break;
            case 15: result = CoroutineManager.javaStreamGenerateNumbers(); break;
            case 16: result = CoroutineManager.cppGeneratorYieldNumbers(); break;
            case 17: result = CoroutineManager.javaStreamGenerateSequence(); break;
            case 18: result = CoroutineManager.cppGeneratorYieldSequence(); break;

            case 19: result = javaSummaryTwoArrayNumbers(); break;
            case 20: result = cppSummaryTwoArrayNumbers(); break;
            case 21: result = javaStreamSummaryTwoArrayNumbers(); break;
            case 22: result = cppSimdSummaryTwoArrayNumbers(); break;
            default: result = "Unknown operation!!!"; break;
        }

        return result;
    }

    private String javaSummaryTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    private String cppSummaryTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    private String javaStreamSummaryTwoArrayNumbers() {
        return "Not implemented yet\n";
    }

    private String cppSimdSummaryTwoArrayNumbers() {
        return "Not implemented yet\n";
    }
}
