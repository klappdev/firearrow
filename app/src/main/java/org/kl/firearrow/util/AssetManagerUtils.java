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
package org.kl.firearrow.util;

import android.content.res.AssetManager;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;

import com.google.gson.stream.JsonReader;
import com.google.gson.Gson;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.lang.reflect.Type;
import java.nio.charset.Charset;

public final class AssetManagerUtils {

    private AssetManagerUtils() throws IllegalAccessException {
        throw new IllegalAccessException("Can't create instance");
    }

    public static <T> List<T> readJsonFileLines(@NonNull AssetManager assetManager, @NonNull String name,
                                                @NonNull Type type, @NonNull Charset charsets) throws Exception {
        if (!name.endsWith(".json")) {
            throw new Exception("File hasn't .json extension");
        }

        try (var inputStream = assetManager.open(name);
             var jsonReader = new JsonReader(new InputStreamReader(inputStream, charsets))) {

            return new ArrayList<>(new Gson().fromJson(jsonReader, type));
        }
    }

    public static <T> List<T> readJsonFileLines(@NonNull AssetManager assetManager, @NonNull String name,
                                                @NonNull Type type) throws Exception {
        return readJsonFileLines(assetManager, name, type, StandardCharsets.UTF_8);
    }

    public static List<String> readFileLines(@NonNull AssetManager assetManager, @NonNull String name,
                                             @NonNull Charset charsets) throws Exception {
        try (var inputStream = assetManager.open(name);
             var bufferedReader = new BufferedReader(new InputStreamReader(inputStream, charsets))) {

            return bufferedReader.lines().collect(Collectors.toCollection(ArrayList::new));
        }
    }

    public static List<String> readFileLines(@NonNull AssetManager assetManager, @NonNull String name) throws Exception {
        return readFileLines(assetManager, name, StandardCharsets.UTF_8);
    }

    public static String readFileLine(@NonNull AssetManager assetManager, @NonNull String name,
                                      @NonNull String delimiter) throws Exception {
        return String.join(delimiter, readFileLines(assetManager, name));
    }

    public static String readFileLine(@NonNull AssetManager assetManager, @NonNull String name) throws Exception {
        return readFileLine(assetManager, name, " ");
    }
}

