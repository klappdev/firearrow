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
#include <jni.h>

#include <logging/Logging.hpp>

JavaVM* globalJavaVm;

extern int registerJniExceptions(JNIEnv*);
extern void unregisterJniExceptions(JNIEnv*);
extern int registerFileManager(JNIEnv *rawEnv);
extern void unregisterFileManager(JNIEnv *rawEnv);
extern int registerCoroutineManager(JNIEnv*);
extern void unregisterCoroutineManager(JNIEnv*);
extern int registerSimdManager(JNIEnv*);
extern void unregisterSimdManager(JNIEnv*);
extern int registerNetworkManager(JNIEnv*);
extern void unregisterNetworkManager(JNIEnv*);

static constexpr jint JNI_DEFAULT_VERSION = JNI_VERSION_1_6;
static constexpr const char* const TAG = "LoadLibrary-JNI";

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* jvm, [[maybe_unused]] void* reserved) {
    JNIEnv* env = nullptr;
    globalJavaVm = jvm;

    if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_DEFAULT_VERSION) != JNI_OK) {
        kl::log::error(TAG, "Could not get JNIEnv");
        return JNI_ERR;
    }

    registerJniExceptions(env);

    if (registerFileManager(env) != 0) {
        kl::log::error(TAG, "Could not register file manager");
        return JNI_ERR;
    }

    if (registerCoroutineManager(env) != 0) {
        kl::log::error(TAG, "Could not register coroutine manager");
        return JNI_ERR;
    }

    if (registerSimdManager(env) != 0) {
        kl::log::error(TAG, "Could not register simd manager");
        return JNI_ERR;
    }

    if (registerNetworkManager(env) != 0) {
        kl::log::error(TAG, "Could not register network manager");
        return JNI_ERR;
    }

    kl::log::info(TAG, "Load jni library");

    return JNI_DEFAULT_VERSION;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *jvm, [[maybe_unused]] void *reserved) {
    JNIEnv* env = nullptr;

    if (jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_DEFAULT_VERSION)) {
        kl::log::error(TAG, "Could not unregister jni components");
        return;
    }

    unregisterJniExceptions(env);
    unregisterFileManager(env);
    unregisterCoroutineManager(env);
    unregisterSimdManager(env);
    unregisterNetworkManager(env);

    kl::log::info(TAG, "Unload jni library");
}
