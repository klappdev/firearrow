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

#include <chrono>
#include <array>

#include "FileEraser.hpp"
#include "OverwriteMode.hpp"

#include <jni/UniqueUtfChars.hpp>
#include <util/nullability/NonNull.hpp>
#include <util/nullability/Nullable.hpp>

namespace {
    jclass fileExceptionClass = nullptr;
    jclass overwriteModeClass = nullptr;

    jfieldID simpleModeFieldId = nullptr;
    jmethodID nameMethodId = nullptr;
}

using namespace kl::util::nullability;

namespace kl::fs {

    jlong nativeEraseFile(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jobject jvmOverwriteMode) {
        auto env = makeNonNull(rawEnv);
        auto& eraser = FileEraser::instance();

        jni::UniqueUtfChars jvmUniquePath(env, jvmPath);
        std::filesystem::path filePath(static_cast<const char*>(jvmUniquePath.get()));

        auto modeName = (jstring) env->CallObjectMethod(jvmOverwriteMode, nameMethodId);
        jni::UniqueUtfChars jvmModeName(env, modeName);

        auto beginTime = std::chrono::steady_clock::now();

        auto overwriteMode = OVERWRITE_MODE.value(jvmModeName.get());

        if (!overwriteMode.has_value()) {
            env->ThrowNew(fileExceptionClass, "Set unknown OverwriteMode");
            return -1LL;
        }

        if (auto result = eraser.init(filePath, *overwriteMode); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return -1LL;
        }

        if (auto result = eraser.checkPermission(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return -1LL;
        }

        if (auto result = eraser.overwriteFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return -1LL;
        }

        if (auto result = eraser.truncateFile(0); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return -1LL;
        }

        if (auto result = eraser.removeFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(fileExceptionClass, message.c_str());
            return -1LL;
        }

        auto endTime = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
    }

    jlong nativeEraseFileWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jstring jvmPath) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseFile(env, clazz, jvmPath, simpleModeObject);
    }

    jlong nativeEraseDirectory(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jobject jvmOverwriteMode, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        const auto jvmUniquePath = jni::UniqueUtfChars(env, jvmPath);
        const auto folder = std::filesystem::path(static_cast<const char*>(jvmUniquePath.get()));

        jstring filePath = nullptr;
        auto beginTime = std::chrono::steady_clock::now();

        if (!std::filesystem::exists(folder)) {
            env->ThrowNew(fileExceptionClass, "Directory doesn't exist");
            return -1LL;
        }

        if (!std::filesystem::is_directory(folder)) {
            env->ThrowNew(fileExceptionClass, "Path doesn't directory");
            return -1LL;
        }

        if (isRecursive) {
            for (const auto& item : std::filesystem::recursive_directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!std::filesystem::is_directory(item.path())) {
                    if (!nativeEraseFile(env, clazz, filePath, jvmOverwriteMode)) {
                        return -1LL;
                    }
                }
            }
        } else {
            for (const auto& item : std::filesystem::directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!nativeEraseFile(env, clazz, filePath, jvmOverwriteMode)) {
                    return -1LL;
                }
            }
        }

        auto endTime = std::chrono::steady_clock::now();

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count();
    }

    jlong nativeEraseDirectoryWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseDirectory(env, clazz, jvmPath, simpleModeObject, isRecursive);
    }

    constexpr std::array<JNINativeMethod, 4> JNI_METHODS = {{
        {"eraseFile", "(Ljava/lang/String;)J", (void*)nativeEraseFileWithDefaultMode},
        {"eraseFile", "(Ljava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;)J", (void*)nativeEraseFile},
        {"eraseDirectory", "(Ljava/lang/String;Z)J", (void*)nativeEraseDirectoryWithDefaultMode},
        {"eraseDirectory", "(Ljava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;Z)J", (void*)nativeEraseDirectory}
    }};
}

jint registerFileManager(JNIEnv* rawEnv) {
    using kl::fs::JNI_METHODS;
    auto env = makeNonNull(rawEnv);

    jclass temporaryClass = env->FindClass("org/kl/firearrow/fs/FileException");
    fileExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/fs/OverwriteMode");
    overwriteModeClass = (jclass) env->NewGlobalRef(temporaryClass);

    simpleModeFieldId = env->GetStaticFieldID(overwriteModeClass, "SIMPLE_MODE", "Lorg/kl/firearrow/fs/OverwriteMode;");
    nameMethodId = env->GetMethodID(overwriteModeClass, "name", "()Ljava/lang/String;");

    jclass fileManagerClass = env->FindClass("org/kl/firearrow/fs/FileManager");
    return env->RegisterNatives(fileManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterFileManager(JNIEnv* rawEnv) {
    auto env = makeNonNull(rawEnv);

    env->DeleteGlobalRef(overwriteModeClass);
    env->DeleteGlobalRef(fileExceptionClass);
}

