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

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.datastore.core.Serializer;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import kotlin.Unit;
import kotlin.coroutines.Continuation;
import timber.log.Timber;

import org.kl.firearrow.Settings;

public final class SettingSerializer implements Serializer<Settings> {
    @Override
    public Settings getDefaultValue() {
        return Settings.getDefaultInstance();
    }

    @Nullable
    @Override
    public Settings readFrom(@NonNull InputStream inputStream, @NonNull Continuation<? super Settings> continuation) {
        try {
            return Settings.parseFrom(inputStream);
        } catch (IOException e) {
            Timber.e("Can't write to *.proto file");
            return getDefaultValue();
        }
    }

    @Nullable
    @Override
    public Object writeTo(Settings settings, @NonNull OutputStream outputStream, @NonNull Continuation<? super Unit> continuation) {
        try {
            settings.writeTo(outputStream);
        } catch (IOException e) {
            Timber.e("Can't write to *.proto file");
        }

        return null;
    }
}
