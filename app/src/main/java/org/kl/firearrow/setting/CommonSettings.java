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
package org.kl.firearrow.setting;

import android.content.Context;
import android.content.res.Configuration;

import androidx.annotation.NonNull;
import androidx.annotation.VisibleForTesting;
import androidx.appcompat.app.AppCompatDelegate;
import androidx.datastore.rxjava3.RxDataStore;
import androidx.datastore.rxjava3.RxDataStoreBuilder;

import io.reactivex.rxjava3.core.Flowable;
import io.reactivex.rxjava3.core.Single;

import lombok.Getter;
import java.util.Locale;

import org.kl.firearrow.R;
import org.kl.firearrow.Settings;

public final class CommonSettings {
    private static final String SETTING_FILE = "settings.proto";
    private static final String SETTING_TEST_FILE = "settings_test.proto";

    private static volatile CommonSettings instance;
    private static volatile CommonSettings instanceTest;
    private final RxDataStore<Settings> dataStore;

    @Getter
    private final String[] availableThemes;
    @Getter
    private final String[] availableLanguages;
    @Getter
    private final String[] availableCodeLanguages;

    private static String DEFAULT_LANGUAGE;
    private static String DEFAULT_THEME;

    private CommonSettings(@NonNull Context context, @NonNull String name) {
        this.dataStore = new RxDataStoreBuilder<>(context, name, new SettingSerializer()).build();

        this.availableThemes = context.getResources().getStringArray(R.array.theme_names);
        this.availableLanguages = context.getResources().getStringArray(R.array.language_full_names);
        this.availableCodeLanguages = context.getResources().getStringArray(R.array.language_short_names);

        DEFAULT_LANGUAGE = availableCodeLanguages[0];
        DEFAULT_THEME = availableThemes[0];
    }

    public static CommonSettings getInstance(@NonNull Context context) {
        if (instance == null) {
            synchronized (CommonSettings.class) {
                if (instance == null) {
                    instance = new CommonSettings(context, SETTING_FILE);
                }
            }
        }
        return instance;
    }

    @VisibleForTesting
    public static CommonSettings getInstanceTest(@NonNull Context context) {
        if (instanceTest == null) {
            synchronized (CommonSettings.class) {
                if (instanceTest == null) {
                    instanceTest = new CommonSettings(context, SETTING_TEST_FILE);
                }
            }
        }
        return instanceTest;
    }

    @NonNull
    public Context setLocale(@NonNull Context context, @NonNull String language) {
        final var locale = new Locale(language);
        Locale.setDefault(locale);

        final Configuration configuration = context.getResources().getConfiguration();
        configuration.setLocale(locale);
        configuration.setLayoutDirection(locale);

        return context.createConfigurationContext(configuration);
    }

    public void setTheme(@NonNull AppCompatDelegate delegate, boolean isDefaultTheme) {
        if (isDefaultTheme) {
            delegate.setLocalNightMode(AppCompatDelegate.MODE_NIGHT_NO);
        } else {
            delegate.setLocalNightMode(AppCompatDelegate.MODE_NIGHT_YES);
        }
    }

    public Single<Settings> writeLanguage(@NonNull String language) {
        return dataStore.updateDataAsync(settings ->
                Single.just(settings.toBuilder()
                    .setLanguage(language)
                    .build()));
    }

    @NonNull
    public String readLanguageSynchronously() {
        final String language = dataStore.data().blockingFirst().getLanguage();
        return !language.isEmpty() ? language : DEFAULT_LANGUAGE;
    }

    public Flowable<String> readLanguage() {
        return dataStore.data().map(settings -> {
            final String language = settings.getLanguage();
            return !language.isEmpty() ? language : DEFAULT_LANGUAGE;
        });
    }

    public Single<Settings> writeTheme(@NonNull String theme) {
        return dataStore.updateDataAsync(settings ->
                Single.just(settings.toBuilder()
                        .setTheme(theme)
                        .build()));
    }

    @NonNull
    public String readThemeSynchronously() {
        final String theme = dataStore.data().blockingFirst().getTheme();
        return !theme.isEmpty() ? theme : DEFAULT_THEME;
    }

    public Flowable<String> readTheme() {
        return dataStore.data().map(settings -> {
            final String theme = settings.getTheme();
            return !theme.isEmpty() ? theme : DEFAULT_THEME;
        });
    }

    public boolean hasDefaultTheme() {
        return readThemeSynchronously().equals(DEFAULT_THEME);
    }
}
