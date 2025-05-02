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
package org.kl.firearrow.fs;

import android.content.Context;
import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileWriter;
import java.lang.ref.Cleaner;

import org.kl.firearrow.time.StopWatch;

public final class FileManager implements AutoCloseable {

    private final static String ERASE_DIRECTORY = "erase";
    private final static String ERASE_FILE1 = "erase1.txt";
    private final static String ERASE_FILE2 = "erase2.txt";
    private final static int COUNT_LINES = 50;

    private long nativeHandle;

    private static final Cleaner cleaner = Cleaner.create();
    private final Cleaner.Cleanable cleanable;
    private final Destructor destructor;

    private final StopWatch stopWatch;

    public FileManager() {
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

    private static native long nativeEraseFile(long nativeHandle, @NonNull String path) throws FileException;

    private static native long nativeEraseFile(long nativeHandle, @NonNull String path, OverwriteMode mode) throws FileException;

    private static native long nativeEraseDirectory(long nativeHandle, @NonNull String path, boolean recursive) throws FileException;

    private static native long nativeEraseDirectory(long nativeHandle, @NonNull String path, OverwriteMode mode, boolean recursive) throws FileException;

    public long eraseFile(@NonNull String path) throws FileException {
        return nativeEraseFile(nativeHandle, path);
    }

    public long eraseFile(@NonNull String path, OverwriteMode mode) throws FileException {
        return nativeEraseFile(nativeHandle, path, mode);
    }

    public long eraseDirectory(@NonNull String path, boolean recursive) throws FileException {
        return nativeEraseDirectory(nativeHandle, path, recursive);
    }

    public long eraseDirectory(@NonNull String path, OverwriteMode mode, boolean recursive) throws FileException {
        return nativeEraseDirectory(nativeHandle, path, mode, recursive);
    }

    public String javaDeleteFile(@NonNull Context context) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java deleting file\n");

        final var directory = createDirectory(context, builder);
        final var file = new File(directory, ERASE_FILE2);
        final var filePath = file.getPath();
        fillFile(file, "Some text", builder);

        builder.append("> Delete file ").append(filePath).append("\n");

        if (!file.delete()) {
            builder.append("> File ").append(filePath).append(" didn't deleted\n");
        }

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppEraseFile(@NonNull Context context, OverwriteMode mode) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ erasing file with ").append(mode.name()).append("\n");

        final var directory = createDirectory(context, builder);
        final var file = new File(directory, ERASE_FILE1);
        final var filePath = file.getPath();
        fillFile(file, "Some text", builder);

        try {
            builder.append("> Erase file ").append(filePath).append("\n");

            final long duration = eraseFile(filePath, mode);

            builder.append("> C++ execution time: ").append(duration).append(" ms\n");
        } catch (FileException e) {
            builder.append("> Erase file ").append(filePath).append(" exception")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String javaDeleteDirectory(@NonNull Context context) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java deleting directory\n");

        final var directory = createDirectory(context, builder);
        final String directoryPath = directory.getPath();

        final var file1 = new File(directory, ERASE_FILE1);
        fillFile(file1, "Some text in file1", builder);

        final var file2 = new File(directory, ERASE_FILE2);
        fillFile(file2, "Some text in file2", builder);

        builder.append("> Delete directory ").append(directoryPath).append("\n");
        deleteDirectory(directory, builder);

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppEraseDirectory(@NonNull Context context) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ erasing directory\n");

        final var directory = createDirectory(context, builder);
        final String directoryPath = directory.getPath();

        final var file1 = new File(directory, ERASE_FILE1);
        fillFile(file1, "Some text in file1", builder);

        final var file2 = new File(directory, ERASE_FILE2);
        fillFile(file2, "Some text in file2", builder);

        try {
            builder.append("> Erase directory ").append(directoryPath).append("\n");

            final long duration = eraseDirectory(directoryPath, false);

            builder.append("> C++ execution time: ").append(duration).append(" ms\n");
        } catch (FileException e) {
            builder.append("> Erase directory ").append(directoryPath).append(" exception")
                   .append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String javaEraseDirectoryRecursive(@NonNull Context context) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart Java deleting directory recursive\n");

        final var directory = createDirectory(context, builder);
        final String directoryPath = directory.getPath();

        final var file1 = new File(directory, ERASE_FILE1);
        fillFile(file1, "Some text in file1", builder);

        final var file2 = new File(directory, ERASE_FILE2);
        fillFile(file2, "Some text in file2", builder);

        builder.append("> Delete directory recursive").append(directoryPath).append("\n");
        deleteDirectoryRecursive(directory, builder);

        stopWatch.stop();
        builder.append("> Java execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }

    public String cppEraseDirectoryRecursive(@NonNull Context context) {
        stopWatch.reset();
        stopWatch.start();

        final var builder = new StringBuilder();
        builder.append("\nStart C++ erasing directory recursive\n");

        final var directory = createDirectory(context, builder);
        final String directoryPath = directory.getPath();

        final var file1 = new File(directory, ERASE_FILE1);
        fillFile(file1, "Some text in file1", builder);

        final var file2 = new File(directory, ERASE_FILE2);
        fillFile(file2, "Some text in file2", builder);

        try {
            builder.append("> Erase directory recursive").append(directoryPath).append("\n");

            final long duration = eraseDirectory(directoryPath, true);

            builder.append("> C++ execution time: ").append(duration).append(" ms\n");
        } catch (FileException e) {
            builder.append("> Erase directory recursive").append(directoryPath)
                   .append(" exception").append(e.getMessage()).append("\n");
        }

        stopWatch.stop();
        builder.append("> JNI execution time: ").append(stopWatch.getDuration()).append(" ms\n");

        return builder.toString();
    }


    private File createDirectory(@NonNull Context context, @NonNull StringBuilder builder) {
        final var directory = new File(context.getFilesDir(), ERASE_DIRECTORY);

        if (!directory.exists()) {
            if (!directory.mkdir()) {
                builder.append("> Directory ").append(ERASE_DIRECTORY).append(" didn't created\n");
            }
        }

        return directory;
    }

    private void fillFile(@NonNull File file, @NonNull String text,
                          @NonNull StringBuilder builder) {
        try {
            final var writer = new FileWriter(file);

            for (int i = 0; i < COUNT_LINES; i++) {
                writer.append(text).append(" ")
                      .append(String.valueOf(i)).append("\n");
            }

            writer.flush();
            writer.close();
        } catch (Exception e) {
            builder.append("> File write exception: ").append(e.getMessage()).append("\n");
        }
    }

    private void deleteDirectory(@NonNull File directory, @NonNull StringBuilder builder) {
        if (!directory.isDirectory()) {
            return;
        }

        final String[] children = directory.list();

        if (children == null) {
            builder.append("> Directory didn't contain files\n");
            return;
        }

        for (String child : children) {
            final var childFile = new File(directory, child);

            if (!childFile.delete()) {
                builder.append("> File ").append(childFile.getPath()).append(" didn't deleted\n");
            }
        }

        if (!directory.delete()) {
            builder.append("> Directory ").append(directory.getPath()).append(" didn't deleted\n");
        }
    }

    private void deleteDirectoryRecursive(@NonNull File fileOrDirectory, @NonNull StringBuilder builder) {
        final File[] children = fileOrDirectory.listFiles();

        if (children != null) {
            for (File child : children) {
                deleteDirectoryRecursive(child, builder);
            }
        }

        if (!fileOrDirectory.delete()) {
            builder.append("> Directory ").append(fileOrDirectory.getPath()).append(" didn't deleted\n");
        }
    }
}
