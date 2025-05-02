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

#include <array>

#include "FileEraser.hpp"
#include "OverwriteMode.hpp"

#include <jni/UniqueUtfChars.hpp>
#include <nullability/NonNull.hpp>
#include <nullability/Nullable.hpp>
#include <time/StopWatch.hpp>

namespace {
    jclass fileExceptionClass = nullptr;
    jclass overwriteModeClass = nullptr;

    jfieldID simpleModeFieldId = nullptr;
    jmethodID nameMethodId = nullptr;
}

namespace firearrow::fs {
    using namespace nullability;
    using namespace time;

    jlong nativeFileEraserCreate(JNIEnv* env, jclass clazz) {
        auto* fileEraser = new FileEraser();
        return reinterpret_cast<jlong>(fileEraser);
    }

    void nativeFileEraserDestroy(JNIEnv* env, jclass clazz, jlong nativeHandle) {
        auto* fileEraser = reinterpret_cast<FileEraser*>(nativeHandle);
        delete fileEraser;
    }

    jlong nativeEraseFile(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmPath, jobject jvmOverwriteMode) {
        auto* fileEraser = reinterpret_cast<FileEraser*>(nativeHandle);

        if (fileEraser == nullptr) {
            return JNI_ERR;
        }

        auto env = makeNonNull(rawEnv);

        jni::UniqueUtfChars jvmUniquePath(env, jvmPath);
        std::filesystem::path filePath(static_cast<const char*>(jvmUniquePath.get()));

        auto modeName = (jstring) env->CallObjectMethod(jvmOverwriteMode, nameMethodId);
        jni::UniqueUtfChars jvmModeName(env, modeName);

        StopWatch stopWatch;
        stopWatch.start();

        auto overwriteMode = OVERWRITE_MODE.value(jvmModeName.get());

        if (!overwriteMode.has_value()) {
            env->ThrowNew(fileExceptionClass, "Set unknown OverwriteMode");
            return JNI_ERR;
        }

        if (auto result = fileEraser->init(filePath, *overwriteMode); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return JNI_ERR;
        }

        if (auto result = fileEraser->checkPermission(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return JNI_ERR;
        }

        if (auto result = fileEraser->overwriteFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return JNI_ERR;
        }

        if (auto result = fileEraser->truncateFile(0); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return JNI_ERR;
        }

        if (auto result = fileEraser->removeFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return JNI_ERR;
        }

        stopWatch.stop();

        return stopWatch.getDuration();
    }

    jlong nativeEraseFileWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmPath) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseFile(env, clazz, nativeHandle, jvmPath, simpleModeObject);
    }

    jlong nativeEraseDirectory(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmPath, jobject jvmOverwriteMode, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        const auto jvmUniquePath = jni::UniqueUtfChars(env, jvmPath);
        const auto folder = std::filesystem::path(static_cast<const char*>(jvmUniquePath.get()));

        jstring filePath = nullptr;
        StopWatch stopWatch;
        stopWatch.start();

        if (!std::filesystem::exists(folder)) {
            env->ThrowNew(fileExceptionClass, "Directory doesn't exist");
            return JNI_ERR;
        }

        if (!std::filesystem::is_directory(folder)) {
            env->ThrowNew(fileExceptionClass, "Path doesn't directory");
            return JNI_ERR;
        }

        if (isRecursive) {
            for (const auto& item : std::filesystem::recursive_directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!std::filesystem::is_directory(item.path())) {
                    if (!nativeEraseFile(env, clazz, nativeHandle, filePath, jvmOverwriteMode)) {
                        return JNI_ERR;
                    }
                }
            }
        } else {
            for (const auto& item : std::filesystem::directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!nativeEraseFile(env, clazz, nativeHandle, filePath, jvmOverwriteMode)) {
                    return JNI_ERR;
                }
            }
        }

        stopWatch.stop();

        return stopWatch.getDuration();
    }

    jlong nativeEraseDirectoryWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmPath, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseDirectory(env, clazz, nativeHandle, jvmPath, simpleModeObject, isRecursive);
    }

    constexpr std::array<JNINativeMethod, 6> JNI_METHODS = {{
        {"nativeCreate", "()J", (void*)nativeFileEraserCreate},
        {"nativeDestroy", "(J)V", (void*)nativeFileEraserDestroy},
        {"nativeEraseFile", "(JLjava/lang/String;)J", (void*)nativeEraseFileWithDefaultMode},
        {"nativeEraseFile", "(JLjava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;)J", (void*)nativeEraseFile},
        {"nativeEraseDirectory", "(JLjava/lang/String;Z)J", (void*)nativeEraseDirectoryWithDefaultMode},
        {"nativeEraseDirectory", "(JLjava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;Z)J", (void*)nativeEraseDirectory}
    }};
}

jint registerFileManager(JNIEnv* env) {
    using firearrow::fs::JNI_METHODS;

    jclass temporaryClass = env->FindClass("org/kl/firearrow/fs/FileException");
    fileExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    if (fileExceptionClass == nullptr) {
        return JNI_ERR;
    }

    temporaryClass = env->FindClass("org/kl/firearrow/fs/OverwriteMode");
    overwriteModeClass = (jclass) env->NewGlobalRef(temporaryClass);

    if (overwriteModeClass == nullptr) {
        return JNI_ERR;
    }

    simpleModeFieldId = env->GetStaticFieldID(overwriteModeClass, "SIMPLE_MODE", "Lorg/kl/firearrow/fs/OverwriteMode;");
    nameMethodId = env->GetMethodID(overwriteModeClass, "name", "()Ljava/lang/String;");

    if (simpleModeFieldId == nullptr || nameMethodId == nullptr) {
        return JNI_ERR;
    }

    jclass fileManagerClass = env->FindClass("org/kl/firearrow/fs/FileManager");

    if (fileManagerClass == nullptr) {
        return JNI_ERR;
    }

    env->DeleteLocalRef(temporaryClass);

    return env->RegisterNatives(fileManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterFileManager(JNIEnv* env) {
    env->DeleteGlobalRef(overwriteModeClass);
    env->DeleteGlobalRef(fileExceptionClass);
}

