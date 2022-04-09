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

import static org.junit.Assert.assertEquals;

import android.content.Context;
import androidx.arch.core.executor.testing.InstantTaskExecutorRule;
import androidx.test.ext.junit.runners.AndroidJUnit4;
import androidx.test.platform.app.InstrumentationRegistry;

import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.runner.RunWith;

import org.kl.firearrow.Settings;

@RunWith(AndroidJUnit4.class)
public class CommonSettingsTest {

    @Rule
    public final InstantTaskExecutorRule taskExecutorRule = new InstantTaskExecutorRule();

    private CommonSettings settings;

    @Before
    public void setUp() {
        final Context context = InstrumentationRegistry.getInstrumentation().getTargetContext();

        this.settings = CommonSettings.getInstanceTest(context);
    }

    @Test
    public void writeAndReadLanguageTest() {
        final String testLanguage = "fr";

        final Settings expected = settings.writeLanguage(testLanguage)
            .doOnError(Throwable::printStackTrace)
            .blockingGet();

        final String actual = settings.readLanguage()
            .doOnError(Throwable::printStackTrace)
            .blockingFirst();

        assertEquals(testLanguage, expected.getLanguage());
        assertEquals(testLanguage, actual);
    }

    @Test
    public void writeAndReadThemeTest() {
        final String testTheme = "bright";

        final Settings expected = settings.writeTheme(testTheme)
            .doOnError(Throwable::printStackTrace)
            .blockingGet();

        final String actual = settings.readTheme()
            .doOnError(Throwable::printStackTrace)
            .blockingFirst();

        assertEquals(testTheme, expected.getTheme());
        assertEquals(testTheme, actual);
    }
}
