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
package org.kl.firearrow.ui.setting;

import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.ListPreference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.SwitchPreferenceCompat;

import javax.inject.Inject;

import java.util.Objects;
import java.util.function.Consumer;

import dagger.hilt.android.AndroidEntryPoint;
import io.reactivex.rxjava3.disposables.CompositeDisposable;

import org.kl.firearrow.R;
import org.kl.firearrow.event.setting.ChangeLanguageListener;
import org.kl.firearrow.event.setting.ChangeThemeListener;

@AndroidEntryPoint
public final class SettingFragment extends PreferenceFragmentCompat {
    @Inject
    public CompositeDisposable disposable;

    private final Consumer<Boolean> updateAction;

    public SettingFragment(@NonNull Consumer<Boolean> updateAction) {
        this.updateAction = updateAction;
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        final var changeLanguageListener = new ChangeLanguageListener(requireContext(), disposable, updateAction);
        final var changeThemeListener = new ChangeThemeListener(requireContext(), disposable, updateAction);

        final var languagePreference = (ListPreference) findPreference("language");
        final var themePreference = (SwitchPreferenceCompat) findPreference("theme");

        if (languagePreference != null && themePreference != null) {
            languagePreference.setOnPreferenceChangeListener(changeLanguageListener);
            themePreference.setOnPreferenceChangeListener(changeThemeListener);
        }
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        disposable.dispose();
    }

    @Override
    public void onCreatePreferences(Bundle savedInstanceState, String rootKey) {
        setPreferencesFromResource(R.xml.app_preferences, rootKey);
    }
}
