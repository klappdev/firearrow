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

#include <backtrace/Backtrace.hpp>
#include "JniException.hpp"
#include "UniqueJniEnv.hpp"

namespace {
    jclass errorClass = nullptr;
    jclass stacktraceElementClass = nullptr;

    jmethodID exceptionConstructorId = nullptr;
    jmethodID causeExceptionConstructorId = nullptr;
    jmethodID stacktraceElementConstructor = nullptr;
    jmethodID setStackTraceMethodId = nullptr;
}

void registerJniExceptions(JNIEnv* env) {
    if (!env) return;

    jclass temporaryClass = nullptr;

    temporaryClass = env->FindClass("java/lang/Error");
    errorClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("java/lang/StackTraceElement");
    stacktraceElementClass = (jclass) env->NewGlobalRef(temporaryClass);

    exceptionConstructorId = env->GetMethodID(errorClass, "<init>", "(Ljava/lang/String;)V");
    causeExceptionConstructorId = env->GetMethodID(errorClass, "<init>", "(Ljava/lang/String;Ljava/lang/Throwable;)V");
    stacktraceElementConstructor = env->GetMethodID(stacktraceElementClass, "<init>", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;I)V");
    setStackTraceMethodId = env->GetMethodID(errorClass, "setStackTrace", "([Ljava/lang/StackTraceElement;)V");
}

void unregisterJniExceptions(JNIEnv* env) {
    if (!env) return;

    env->DeleteGlobalRef(errorClass);
    env->DeleteGlobalRef(stacktraceElementClass);
}

namespace kl::jni {
    using namespace kl::backtrace;

    static void prepareStacktraceElement(JNIEnv* env, const Backtrace& backtrace, jobjectArray elements) {
        jsize index = 0;

        for (const auto& [fileName, functionName, address, offset] : backtrace) {
            jstring jvmDeclaringClass = env->NewStringUTF(fileName.c_str());
            jstring jvmMethodName = env->NewStringUTF(format("%p", (void*) address).c_str());
            jstring jvmFileName = env->NewStringUTF(format("%s:%p", functionName.c_str(), (void*) offset).c_str());
            const jint lineNumber = !functionName.empty() ? 0 : -2;

            jobject trace = env->NewObject(stacktraceElementClass, stacktraceElementConstructor,
                                           jvmDeclaringClass, jvmMethodName, jvmFileName, lineNumber);
            env->SetObjectArrayElement(elements, index++, trace);
        }
    }

    void jvmThrowException(const std::string& message) {
        UniqueJniEnv uniqueJniEnv;
        JNIEnv* env = uniqueJniEnv.get();

        if (!env) return;
        if (env->ExceptionCheck()) return;

        jstring jvmMessage = env->NewStringUTF(message.c_str());
        jthrowable exception = nullptr;

        /*registerJniExceptions(env);*/

        auto backtrace = Backtrace::current(/*skip=*/4);

        if (!backtrace.empty()) {
            auto cause = (jthrowable) env->NewObject(errorClass, exceptionConstructorId, jvmMessage);
            jobjectArray elements = env->NewObjectArray(backtrace.size(), stacktraceElementClass, nullptr);

            if (elements != nullptr) {
                prepareStacktraceElement(env, backtrace, elements);
                env->CallVoidMethod(cause, setStackTraceMethodId, elements);
            }

            exception = (jthrowable) env->NewObject(errorClass, causeExceptionConstructorId, jvmMessage, cause);
        } else {
            exception = (jthrowable) env->NewObject(errorClass, exceptionConstructorId, jvmMessage);
        }

        if (exception != nullptr) {
            env->Throw(exception);
        } else {
            env->ThrowNew(errorClass, message.c_str());
        }
    }
}

