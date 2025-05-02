/*
 * Licensed under the MIT License <http://opensource.org/licenses/MIT>.
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2022-2025 https://github.com/klappdev
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
package org.kl.firearrow.net;

import androidx.annotation.NonNull;

import org.kl.firearrow.time.StopWatch;

import java.io.IOException;
import java.lang.ref.Cleaner;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

public final class NetworkManager implements AutoCloseable {

    private static final String HOST_NAME = "https://example.com";
    private static final int HOST_PORT = 80;

    private long nativeHandle;

    private static final Cleaner cleaner = Cleaner.create();
    private final Cleaner.Cleanable cleanable;
    private final Destructor destructor;

    private final StopWatch stopWatch;

    public NetworkManager() {
        if (nativeHandle == 0) {
            nativeHandle = nativeCreate();
        }

        this.destructor = new Destructor(nativeHandle);
        this.cleanable = cleaner.register(this, destructor);
        this.stopWatch = new StopWatch();
    }

    private static class Destructor implements Runnable {
        private long nativeHandle;

        private Destructor(long nativeHandle) {
            this.nativeHandle = nativeHandle;
        }

        @Override
        public void run() {
            if (nativeHandle != 0) {
                nativeDestroy(nativeHandle);
                nativeHandle = 0;
            }
        }
    }

    @Override
    public void close() {
        cleanable.clean();
    }

    private static native long nativeCreate();

    private static native void nativeDestroy(long nativeHandle);

    private static native NetworkResult nativePerformHttpRequest(long nativeHandle, @NonNull String url, int port) throws NetworkException;

    private static native NetworkResult nativePerformAsyncHttpRequest(long nativeHandle, @NonNull String url, int port) throws NetworkException;

    public NetworkResult performHttpRequest(@NonNull String url, int port) throws NetworkException {
        return nativePerformHttpRequest(nativeHandle, url, port);
    }

    public NetworkResult performAsyncHttpRequest(@NonNull String url, int port) throws NetworkException {
        return nativePerformAsyncHttpRequest(nativeHandle, url, port);
    }

    public String javaPerformHttpRequest() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java perform HTTP request, url: ").append(HOST_NAME).append("\n");

        try {
            final var url = new URL(HOST_NAME);
            final var urlConnection = url.openConnection();

            urlConnection.connect();

            builder.append("> HTTP header fields:\n");

            final Map<String, List<String>> headers = urlConnection.getHeaderFields();

            for (Map.Entry<String, List<String>> headerEntry : headers.entrySet()) {
                final String key = headerEntry.getKey();

                for (String value : headerEntry.getValue()) {
                    builder.append("> Header key: ").append(key).append(",").append(value).append("\n");
                }
            }

            String contentEncoding = urlConnection.getContentEncoding();

            if (contentEncoding == null) {
                contentEncoding = "UTF-8";
            }

            builder.append("> HTTP header content-type: ").append(urlConnection.getContentType()).append("\n");
            builder.append("> HTTP header content-length: ").append(urlConnection.getContentLength()).append("\n");
            builder.append("> HTTP header content-encoding: ").append(contentEncoding).append("\n");
            builder.append("> HTTP header date: ").append(urlConnection.getDate()).append("\n");
            builder.append("> HTTP header expiration: ").append(urlConnection.getExpiration()).append("\n");
            builder.append("> HTTP header last-modified: ").append(urlConnection.getLastModified()).append("\n");

            builder.append("> HTTP response:\n");

            try (final var scanner = new Scanner(urlConnection.getInputStream(), contentEncoding)) {
                while (scanner.hasNextLine()) {
                    builder.append("> ").append(scanner.nextLine()).append("\n");
                }
            }
        } catch (MalformedURLException e) {
            builder.append("> Perform Java HTTP request, url: ").append(HOST_NAME).append(", MalformedURLException: ")
                   .append(e.getMessage()).append("\n");
        } catch (IOException e) {
            builder.append("> Perform Java HTTP request, url: ").append(HOST_NAME).append(", IOException: ")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppPerformHttpRequest() {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ perform HTTP request, url: ").append(HOST_NAME).append("\n");

        try {
            final NetworkResult result = performHttpRequest(HOST_NAME, HOST_PORT);

            builder.append("> C++ HTTP response: ").append(result.value());
            builder.append("> C++ execution time: ").append(result.duration()).append(" ms\n");
        } catch (NetworkException e) {
            builder.append("> Perform HTTP request, url: ").append(HOST_NAME).append(", NetworkException: ")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String javaPerformAsyncHttpRequest() {
        return "Not implemented yet\n";
    }

    public String cppPerformAsyncHttpRequest() {
        return "Not implemented yet\n";
    }
}
