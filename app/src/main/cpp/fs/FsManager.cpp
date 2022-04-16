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

#if __has_include(<filesystem>)
#   include <filesystem>
#else
#   error "missing header <filesystem>"
#endif

#include <array>
#include <jni.h>

#include "FsEraser.hpp"
#include "OverwriteMode.hpp"

#include <jni/UniqueUtfChars.hpp>
#include <util/nullability/NonNull.hpp>
#include <util/nullability/Nullable.hpp>

namespace {
    jclass eraseExceptionClass = nullptr;
    jclass overwriteModeClass = nullptr;

    jfieldID simpleModeFieldId = nullptr;
    jmethodID nameMethodId = nullptr;
}

using namespace kl::util::nullability;

namespace kl::fs {

    static FsEraser& getEraser() {
        static FsEraser eraser;
        return eraser;
    }

    jboolean nativeEraseFile(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jobject jvmOverwriteMode) {
        auto env = makeNonNull(rawEnv);
        const auto jvmUniquePath = jni::UniqueUtfChars(env, jvmPath);
        const auto filePath = std::filesystem::path(static_cast<const char*>(jvmUniquePath.get()));

        const auto modeName = (jstring) env->CallObjectMethod(jvmOverwriteMode, nameMethodId);
        const auto jvmModeName = jni::UniqueUtfChars(env, modeName);

        if (auto result = getEraser().init(filePath, overwriteMode.value(jvmModeName.get())); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(eraseExceptionClass, message.c_str());
            return false;
        }

        if (auto result = getEraser().checkPermission(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(eraseExceptionClass, message.c_str());
            return false;
        }

        if (auto result = getEraser().overwriteFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(eraseExceptionClass, message.c_str());
            return false;
        }

        if (auto result = getEraser().truncateFile(0); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(eraseExceptionClass, message.c_str());
            return false;
        }

        if (auto result = getEraser().removeFile(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(eraseExceptionClass, message.c_str());
            return false;
        }

        return true;
    }

    jboolean nativeEraseFileWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jstring jvmPath) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseFile(env, clazz, jvmPath, simpleModeObject);
    }

    jboolean eraseFiles(JNIEnv* rawEnv, jclass clazz, jobject jvmOverwriteMode, jobjectArray jvmPaths) {
        auto env = makeNonNull(rawEnv);
        size_t length = env->GetArrayLength(jvmPaths);

        for (size_t i = 0; i < length; i++) {
            auto jvmPath = (jstring) env->GetObjectArrayElement(jvmPaths, i);

            if (!nativeEraseFile(env, clazz, jvmPath, jvmOverwriteMode)) {
                return false;
            }
        }

        return true;
    }

    jboolean nativeEraseFilesWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jobjectArray jvmPaths) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return eraseFiles(env, clazz, simpleModeObject, jvmPaths);
    }

    jboolean nativeEraseDirectory(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jobject jvmOverwriteMode, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        const auto jvmUniquePath = jni::UniqueUtfChars(env, jvmPath);
        const auto folder = std::filesystem::path(static_cast<const char*>(jvmUniquePath.get()));
        jstring filePath = nullptr;

        if (!std::filesystem::exists(folder)) {
            env->ThrowNew(eraseExceptionClass, "Directory doesn't exist");
            return false;
        }

        if (!std::filesystem::is_directory(folder)) {
            env->ThrowNew(eraseExceptionClass, "Path doesn't directory");
            return false;
        }

        if (isRecursive) {
            for (const auto& item : std::filesystem::recursive_directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!std::filesystem::is_directory(item.path())) {
                    if (!nativeEraseFile(env, clazz, filePath, jvmOverwriteMode)) {
                        return false;
                    }
                }
            }
        } else {
            for (const auto& item : std::filesystem::directory_iterator(folder)) {
                filePath = env->NewStringUTF(item.path().c_str());

                if (!nativeEraseFile(env, clazz, filePath, jvmOverwriteMode)) {
                    return false;
                }
            }
        }

        return true;
    }

    jboolean nativeEraseDirectoryWithDefaultMode(JNIEnv* rawEnv, jclass clazz, jstring jvmPath, jboolean isRecursive) {
        auto env = makeNonNull(rawEnv);
        jobject simpleModeObject = env->GetStaticObjectField(overwriteModeClass, simpleModeFieldId);

        return nativeEraseDirectory(env, clazz, jvmPath, simpleModeObject, isRecursive);
    }

    const std::array<JNINativeMethod, 6> JNI_METHODS = {{
        {"eraseFile", "(Ljava/lang/String;)Z", (void*)nativeEraseFileWithDefaultMode},
        {"eraseFile", "(Ljava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;)Z", (void*)nativeEraseFile},
        {"eraseFiles", "([Ljava/lang/String;)Z", (void*)nativeEraseFilesWithDefaultMode},
        {"eraseFiles", "(Lorg/kl/firearrow/fs/OverwriteMode;[Ljava/lang/String;)Z", (void*)eraseFiles},
        {"eraseDirectory", "(Ljava/lang/String;Z)Z", (void*)nativeEraseDirectoryWithDefaultMode},
        {"eraseDirectory", "(Ljava/lang/String;Lorg/kl/firearrow/fs/OverwriteMode;Z)Z", (void*)nativeEraseDirectory}
    }};
}

jint registerFilesystemManager(JNIEnv* rawEnv) {
    using kl::fs::JNI_METHODS;

    auto env = makeNonNull(rawEnv);
    jclass temporaryClass;

    temporaryClass = env->FindClass("org/kl/firearrow/fs/EraseException");
    eraseExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/fs/OverwriteMode");
    overwriteModeClass = (jclass) env->NewGlobalRef(temporaryClass);

    simpleModeFieldId = env->GetStaticFieldID(overwriteModeClass, "SIMPLE_MODE", "Lorg/kl/firearrow/fs/OverwriteMode;");
    nameMethodId = env->GetMethodID(overwriteModeClass, "name", "()Ljava/lang/String;");

    jclass fsManagerClass = env->FindClass("org/kl/firearrow/fs/FileSystemManager");
    return env->RegisterNatives(fsManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterFilesystemManager(JNIEnv* rawEnv) {
    auto env = makeNonNull(rawEnv);

    env->DeleteGlobalRef(overwriteModeClass);
    env->DeleteGlobalRef(eraseExceptionClass);
}

