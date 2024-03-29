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
package org.kl.firearrow.ui;

import android.content.Context;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;

import androidx.activity.result.ActivityResultLauncher;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import dagger.hilt.android.AndroidEntryPoint;

import org.kl.firearrow.R;
import org.kl.firearrow.setting.CommonSettings;
import org.kl.firearrow.ui.setting.SettingActivityContract;

@AndroidEntryPoint
public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("firearrow");
    }

    private CommonSettings settings;

    private final ActivityResultLauncher<Void> settingLauncher = registerForActivityResult(
            new SettingActivityContract(), result -> {
        if (result != null && result) {
            MainActivity.this.recreate();
        }
    });

    @Override
    protected void attachBaseContext(@NonNull Context base) {
        this.settings = CommonSettings.getInstance(base);
        final String language = settings.readLanguageSynchronously();

        super.attachBaseContext(settings.setLocale(base, language));
    }

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        settings.setTheme(getDelegate(), settings.hasDefaultTheme());

        setContentView((R.layout.activity_main));
    }

    @Override
    public boolean onCreateOptionsMenu(@Nullable Menu menu) {
        final var inflater = getMenuInflater();
        inflater.inflate(R.menu.menu_main, menu);

        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        final int id = item.getItemId();

        if (id == R.id.action_settings) {
            settingLauncher.launch(null);
        }

        return super.onOptionsItemSelected(item);
    }
}