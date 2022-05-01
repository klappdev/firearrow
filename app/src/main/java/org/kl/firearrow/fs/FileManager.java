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
package org.kl.firearrow.fs;

public final class FileManager {

    private FileManager() throws IllegalAccessException {
        throw new IllegalAccessException("Can't create instance");
    }

    public static native long eraseFile(String path) throws FileException;

    public static native long eraseFile(String path, OverwriteMode mode) throws FileException;

    public static native long eraseDirectory(String path, boolean recursive) throws FileException;

    public static native long eraseDirectory(String path, OverwriteMode mode, boolean recursive) throws FileException;

    public static String cppEraseFile(OverwriteMode mode) {
        final String tmpFile = "/path/file.txt";

        final var builder = new StringBuilder();
        final long beginTime = System.currentTimeMillis();

        builder.append("Start native erasing ").append(tmpFile).append("\n");

        try {
            final long duration = FileManager.eraseFile(tmpFile, mode);

            builder.append("> C++ execution time: ").append(duration).append(" ms\n");
        } catch (FileException e) {
            builder.append("> Erase file ").append(tmpFile).append(" exception")
                   .append(e.getMessage()).append("\n");
        }

        final long endTime = System.currentTimeMillis();
        builder.append("> JNI execution time: ").append(endTime - beginTime).append(" ms\n");

        return builder.toString();
    }

    public static String javaDeleteFile() {
        return "Not implemented yet\n";
    }

    public static String javaDeleteDirectory() {
        return "Not implemented yet\n";
    }

    public static String cppEraseDirectory() {
        return "Not implemented yet\n";
    }

    public static String javaEraseDirectoryRecursive() {
        return "Not implemented yet\n";
    }

    public static String cppEraseDirectoryRecursive() {
        return "Not implemented yet\n";
    }
}
