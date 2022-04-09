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
package org.kl.firearrow.event.setting;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.preference.Preference;

import io.reactivex.rxjava3.android.schedulers.AndroidSchedulers;
import io.reactivex.rxjava3.disposables.CompositeDisposable;
import io.reactivex.rxjava3.observers.DisposableSingleObserver;
import io.reactivex.rxjava3.schedulers.Schedulers;

import org.kl.firearrow.Settings;
import org.kl.firearrow.setting.CommonSettings;

import static org.kl.firearrow.util.ContextUtils.*;

import java.util.function.Consumer;

public final class ChangeThemeListener implements Preference.OnPreferenceChangeListener {
    private final Context context;
    private final CompositeDisposable disposable;
    private final Consumer<Boolean> updateAction;
    private String oldTheme;

    public ChangeThemeListener(@NonNull Context context, @NonNull CompositeDisposable disposable,
                               @NonNull Consumer<Boolean> updateAction) {
        this.context = context;
        this.disposable = disposable;
        this.updateAction = updateAction;
    }

    @Override
    public boolean onPreferenceChange(Preference preference, Object newValue) {
        final var settings = CommonSettings.getInstance(context);
        final var themes = settings.getAvailableThemes();
        final var theme = (boolean) newValue ? themes[0] : themes[1];

        disposable.add(settings.writeTheme(theme)
            .subscribeOn(Schedulers.io())
            .observeOn(AndroidSchedulers.mainThread())
            .subscribeWith(new DisposableSingleObserver<Settings>() {
                @Override public void onSuccess(@NonNull Settings settings) {
                    final String newTheme = settings.getTheme();

                    if (!theme.isEmpty() && !newTheme.equals(oldTheme)) {
                        updateAction.accept(true);
                        toast(context, "Change theme " + newTheme + " successfully");
                    } else {
                        updateAction.accept(false);
                    }

                    oldTheme = newTheme;
                }
                @Override public void onError(@NonNull Throwable e) {
                    toast(context, "Can't change theme: " + theme + " " + e.getMessage());
                }
            }));

        return true;
    }
}
