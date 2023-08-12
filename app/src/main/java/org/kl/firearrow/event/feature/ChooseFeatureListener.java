/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2023 https://github.com/klappdev
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

import android.content.Context;
import android.view.View;

import androidx.annotation.NonNull;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.core.Observable;
import io.reactivex.rxjava3.schedulers.Schedulers;

import org.kl.firearrow.coroutine.CoroutineManager;
import org.kl.firearrow.fs.FileManager;
import org.kl.firearrow.fs.OverwriteMode;
import org.kl.firearrow.net.NetworkManager;
import org.kl.firearrow.simd.SimdManager;
import org.kl.firearrow.ui.feature.FeatureActivity;

public final class ChooseFeatureListener implements View.OnClickListener {
    private final long featureId;
    private final FeatureActivity activity;

    public ChooseFeatureListener(long featureId, @NonNull FeatureActivity activity) {
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
        final Context context = activity.getApplicationContext();

        return switch ((int) featureId) {
            case 1 -> FileManager.javaDeleteFile(context);
            case 2 -> FileManager.cppEraseFile(context, OverwriteMode.SIMPLE_MODE);
            case 3 -> FileManager.cppEraseFile(context, OverwriteMode.OPENBSD_MODE);
            case 4 -> FileManager.cppEraseFile(context, OverwriteMode.DOD_MODE);
            case 5 -> FileManager.javaDeleteDirectory(context);
            case 6 -> FileManager.cppEraseDirectory(context);
            case 7 -> FileManager.javaEraseDirectoryRecursive(context);
            case 8 -> FileManager.cppEraseDirectoryRecursive(context);
            case 9 -> CoroutineManager.javaThreadRunnableOperation();
            case 10 -> CoroutineManager.cppCoroutineRunnableOperation();
            case 11 -> CoroutineManager.javaThreadCallableOperation();
            case 12 -> CoroutineManager.cppCoroutineCallableOperation();
            case 13 -> CoroutineManager.javaStreamGenerateNumbers();
            case 14 -> CoroutineManager.cppGeneratorYieldNumbers();
            case 15 -> CoroutineManager.javaStreamGenerateSequence();
            case 16 -> CoroutineManager.cppGeneratorYieldSequence();
            case 17 -> SimdManager.javaAddTwoArrayNumbers();
            case 18 -> SimdManager.cppAddTwoArrayNumbers();
            case 19 -> SimdManager.javaStreamAddTwoArrayNumbers();
            case 20 -> SimdManager.cppSimdAddTwoArrayNumbers();
            case 21 -> NetworkManager.javaPerformGETRequest();
            case 22 -> NetworkManager.cppPerformGETRequest();
            case 23 -> NetworkManager.javaPerformAsyncGETRequest();
            case 24 -> NetworkManager.cppPerformAsyncGETRequest();
            default -> "Unknown operation!!!";
        };
    }
}
