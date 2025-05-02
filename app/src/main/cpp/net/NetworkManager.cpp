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

#include "HttpClient.hpp"

#include <jni/UniqueJniEnv.hpp>
#include <jni/UniqueUtfChars.hpp>
#include <nullability/NonNull.hpp>
#include <time/StopWatch.hpp>

namespace {
    jclass networkExceptionClass = nullptr;
    jclass networkResultClass = nullptr;

    jmethodID networkResultConstructorId = nullptr;
}

namespace firearrow::net {
    using namespace nullability;
    using namespace time;

    jlong nativeHttpClientCreate(JNIEnv* rawEnv, jclass clazz) {
        auto* httpClient = new HttpClient();
        return reinterpret_cast<jlong>(httpClient);
    }

    void nativeHttpClientDestroy(JNIEnv* env, jclass clazz, jlong nativeHandle) {
        auto* httpClient = reinterpret_cast<HttpClient*>(nativeHandle);
        delete httpClient;
    }

    jobject nativePerformHttpRequest(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmUrl, jint jvmPort) {
        auto* httpClient = reinterpret_cast<HttpClient*>(nativeHandle);

        if (httpClient == nullptr) {
            return nullptr;
        }

        jni::UniqueJniEnv env;
        jni::UniqueUtfChars jvmUniqueUrl(rawEnv, jvmUrl);

        StopWatch stopWatch;
        stopWatch.start();

        if (auto result = httpClient->create(static_cast<const char*>(jvmUniqueUrl.get()), jvmPort); result.hasError()) {
            const std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        if (auto result = httpClient->connect(); result.hasError()) {
            const std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        if (auto result = httpClient->send(); result.hasError()) {
            const std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        Result<std::vector<std::string>, NetworkError> result = httpClient->receive();

        if (result.hasError()) {
            const std::string& message = result.error().message;
            env->ThrowNew(networkExceptionClass, message.c_str());
            return nullptr;
        }

        std::vector<std::string> lines = result.value();
        std::string rawResult;

        /*FIXME: using std::ranges*/
        for (const std::string& line : lines) {
            if (line.starts_with("{") && line.ends_with("}")) {
                rawResult = line;
                break;
            }
        }

        stopWatch.stop();

        return env->NewObject(networkResultClass, networkResultConstructorId,
                              env->NewStringUTF(rawResult.c_str()), stopWatch.getDuration());
    }

    jobject nativePerformAsyncHttpRequest(JNIEnv* rawEnv, jclass clazz, jlong nativeHandle, jstring jvmUrl, jint jvmPort) {
        /*FIXME: implement in future*/
        return nullptr;
    }

    constexpr std::array<JNINativeMethod, 4> JNI_METHODS = {{
        {"nativeCreate", "()J", (void*)nativeHttpClientCreate},
        {"nativeDestroy", "(J)V", (void*)nativeHttpClientDestroy},
        {"nativePerformHttpRequest",
         "(JLjava/lang/String;I)Lorg/kl/firearrow/net/NetworkResult;",
         (void*)nativePerformHttpRequest},
        {"nativePerformAsyncHttpRequest",
         "(JLjava/lang/String;I)Lorg/kl/firearrow/net/NetworkResult;",
         (void*)nativePerformAsyncHttpRequest}
    }};
}

jint registerNetworkManager(JNIEnv* env) {
    using firearrow::net::JNI_METHODS;

    jclass temporaryClass = env->FindClass("org/kl/firearrow/net/NetworkException");
    networkExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    if (networkExceptionClass == nullptr) {
        return JNI_ERR;
    }

    temporaryClass = env->FindClass("org/kl/firearrow/net/NetworkResult");
    networkResultClass = (jclass) env->NewGlobalRef(temporaryClass);

    if (networkResultClass == nullptr) {
        return JNI_ERR;
    }

    networkResultConstructorId = env->GetMethodID(networkResultClass, "<init>", "(Ljava/lang/String;J)V");

    if (networkResultConstructorId == nullptr) {
        return JNI_ERR;
    }

    jclass networkManagerClass = env->FindClass("org/kl/firearrow/net/NetworkManager");

    if (networkManagerClass == nullptr) {
        return JNI_ERR;
    }

    env->DeleteLocalRef(temporaryClass);

    return env->RegisterNatives(networkManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterNetworkManager(JNIEnv* env) {
    env->DeleteGlobalRef(networkExceptionClass);
    env->DeleteGlobalRef(networkResultClass);
}