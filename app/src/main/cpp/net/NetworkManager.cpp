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

#include "Socket.hpp"

#include <jni/UniqueUtfChars.hpp>
#include <util/nullability/NonNull.hpp>

namespace {
    jclass networkExceptionClass = nullptr;
    jclass networkResultClass = nullptr;

    jmethodID networkResultConstructorId = nullptr;
}

using namespace kl::util::nullability;

namespace kl::net {

    jobject nativePerformGETRequest(JNIEnv* rawEnv, jclass clazz, jstring jvmUrl, jint jvmPort) {
        auto env = makeNonNull(rawEnv);
        auto beginTime = std::chrono::steady_clock::now();

        jni::UniqueUtfChars jvmUniqueUrl(env, jvmUrl);
        Socket socket(static_cast<const char*>(jvmUniqueUrl.get()), jvmPort);

        if (auto result = socket.create(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        if (auto result = socket.connect(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        if (auto result = socket.send(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        if (auto result = socket.receive(); result.hasError()) {
            std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        } else {
            std::vector<std::string> lines = result.value();
            std::string rawResult;

            /*FIXME: using std::ranges*/
            for (const std::string& line : lines) {
                if (line.starts_with("{") && line.ends_with("}")) {
                    rawResult = line;
                    break;
                }
            }

            auto endTime = std::chrono::steady_clock::now();

            return env->NewObject(networkResultClass, networkResultConstructorId,
                  env->NewStringUTF(rawResult.c_str()),
                  std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
        }
    }

    jobject nativePerformAsyncGETRequest(JNIEnv* rawEnv, jclass clazz, jstring jvmUrl, jint jvmPort) {
        /*FIXME: implement in future*/
        return nullptr;
    }

    constexpr std::array<JNINativeMethod, 2> JNI_METHODS = {{
        {"performGETRequest",
         "(Ljava/lang/String;I)Lorg/kl/firearrow/net/NetworkResult;",
         (void*)nativePerformGETRequest},
        {"performAsyncGETRequest",
         "(Ljava/lang/String;I)Lorg/kl/firearrow/net/NetworkResult;",
         (void*)nativePerformAsyncGETRequest}
    }};
}

jint registerNetworkManager(JNIEnv* rawEnv) {
    using kl::net::JNI_METHODS;
    auto env = makeNonNull(rawEnv);

    jclass temporaryClass = env->FindClass("org/kl/firearrow/net/NetworkException");
    networkExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/net/NetworkResult");
    networkResultClass = (jclass) env->NewGlobalRef(temporaryClass);

    networkResultConstructorId = env->GetMethodID(networkResultClass, "<init>", "(Ljava/lang/String;J)V");

    jclass networkManagerClass = env->FindClass("org/kl/firearrow/net/NetworkManager");
    return env->RegisterNatives(networkManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterNetworkManager(JNIEnv* rawEnv) {
    auto env = makeNonNull(rawEnv);

    env->DeleteGlobalRef(networkExceptionClass);
    env->DeleteGlobalRef(networkResultClass);
}

