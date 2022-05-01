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

#include <jni.h>
#include <util/nullability/NonNull.hpp>
#include <util/nullability/Nullable.hpp>

#include "Task.hpp"
#include "Generator.hpp"
#include "FuturePromise.hpp"

namespace {
    jclass generatorClass = nullptr;
    jclass taskClass = nullptr;
    jclass coroutineExceptionClass = nullptr;
    jclass callableClass = nullptr;
    jclass runnableClass = nullptr;

    jclass integerClass = nullptr;

    jmethodID generatorConstructorId = nullptr;
    jmethodID taskConstructorId = nullptr;
    jmethodID integerConstructorId = nullptr;

    jmethodID intValueMethodId = nullptr;

    jmethodID callMethodId = nullptr;
    jmethodID runMethodId = nullptr;
}

using namespace kl::util::nullability;

namespace kl::coroutine {

    template<typename T>
    std::future<Task<T>> executeTask(std::function<T()> callback) {
        Task<T> task(std::move(callback));
        co_await task;
        co_return task;
    }

    //FIXME: using concept std::integral
    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    Generator<T> executeGenerator(T init_value, int begin, int end) {
        for (int i = begin; i < end; i++) {
            co_yield (init_value + i);
        }
    }

    jobject nativeAwaitRunnable(JNIEnv* rawEnv, jclass clazz, jobject jvmRunnable) {
        auto env = makeNonNull(rawEnv);
        auto beginTime = std::chrono::steady_clock::now();

        std::future<Task<void>> future = executeTask<void>(
                [&]() {  env->CallVoidMethod(jvmRunnable, runMethodId); });
        Task<void> task = future.get();

        auto endTime = std::chrono::steady_clock::now();

        return env->NewObject(taskClass, taskConstructorId,
              nullptr, task.finished ? JNI_TRUE : JNI_FALSE,
              std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
    }

    jobject nativeAwaitCallable(JNIEnv* rawEnv, jclass clazz, jobject jvmCallable) {
        auto env = makeNonNull(rawEnv);
        auto beginTime = std::chrono::steady_clock::now();

        std::future<Task<Nullable<jobject>>> future = executeTask<Nullable<jobject>>(
                [&]() { return env->CallObjectMethod(jvmCallable, callMethodId); });
        Task<Nullable<jobject>> task = future.get();

        auto endTime = std::chrono::steady_clock::now();

        return env->NewObject(taskClass, taskConstructorId,
                task.value.get(), task.finished ? JNI_TRUE : JNI_FALSE,
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
    }

    jobject nativeYieldInRange(JNIEnv* rawEnv, jclass clazz, jobject jvmInitValue, jint jvmBegin, jint jvmEnd) {
        auto env = makeNonNull(rawEnv);
        auto beginTime = std::chrono::steady_clock::now();

        if (!env->IsInstanceOf(jvmInitValue, integerClass)) {
            env->ThrowNew(coroutineExceptionClass, "Generator could work only with integer initial value");
            return nullptr;
        }

        jint initValue = env->CallIntMethod(jvmInitValue, intValueMethodId);
        jobjectArray jvmNumbers = env->NewObjectArray(jvmEnd - jvmBegin, integerClass, nullptr);

        auto generator = executeGenerator(initValue, jvmBegin, jvmEnd);

        for (std::int32_t i = 0; std::int32_t number : generator) {
            jobject jvmNumber = env->NewObject(integerClass, integerConstructorId, number);
            env->SetObjectArrayElement(jvmNumbers, i++, jvmNumber);
        }

        if (generator.hasError()) {
            env->ThrowNew(coroutineExceptionClass, "Generator take native coroutine error");
            return nullptr;
        }

        auto endTime = std::chrono::steady_clock::now();

        return env->NewObject(generatorClass, generatorConstructorId, jvmNumbers,
                  std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
    }

    jobject nativeYield(JNIEnv* rawEnv, jclass clazz, jobject jvmInitValue, jint jvmCount) {
        return nativeYieldInRange(rawEnv, clazz, jvmInitValue, 0, jvmCount);
    }

    constexpr std::array<JNINativeMethod, 4> JNI_METHODS = {{
        {"await", "(Ljava/lang/Runnable;)Lorg/kl/firearrow/coroutine/Task;", (void*)nativeAwaitRunnable},
        {"await", "(Ljava/util/concurrent/Callable;)Lorg/kl/firearrow/coroutine/Task;", (void*)nativeAwaitCallable},
        {"yield", "(Ljava/lang/Number;I)Lorg/kl/firearrow/coroutine/Generator;", (void*)nativeYield},
        {"yield", "(Ljava/lang/Number;II)Lorg/kl/firearrow/coroutine/Generator;", (void*)nativeYieldInRange},
    }};
}

jint registerCoroutineManager(JNIEnv* rawEnv) {
    using kl::coroutine::JNI_METHODS;

    auto env = makeNonNull(rawEnv);
    jclass temporaryClass;

    temporaryClass = env->FindClass("org/kl/firearrow/coroutine/CoroutineException");
    coroutineExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("java/util/concurrent/Callable");
    callableClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("java/lang/Runnable");
    runnableClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/coroutine/Generator");
    generatorClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/coroutine/Task");
    taskClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("java/lang/Integer");
    integerClass = (jclass) env->NewGlobalRef(temporaryClass);

    generatorConstructorId = env->GetMethodID(generatorClass, "<init>", "([Ljava/lang/Number;J)V");
    taskConstructorId = env->GetMethodID(taskClass, "<init>", "(Ljava/lang/Object;ZJ)V");
    integerConstructorId = env->GetMethodID(integerClass, "<init>", "(I)V");

    intValueMethodId = env->GetMethodID(integerClass, "intValue", "()I");

    callMethodId = env->GetMethodID(callableClass, "call", "()Ljava/lang/Object;");
    runMethodId = env->GetMethodID(runnableClass, "run", "()V");

    jclass coroutineManagerClass = env->FindClass("org/kl/firearrow/coroutine/CoroutineManager");
    return env->RegisterNatives(coroutineManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterCoroutineManager(JNIEnv* rawEnv) {
    auto env = makeNonNull(rawEnv);

    env->DeleteGlobalRef(coroutineExceptionClass);
    env->DeleteGlobalRef(callableClass);
    env->DeleteGlobalRef(runnableClass);

    env->DeleteGlobalRef(integerClass);

    env->DeleteGlobalRef(generatorClass);
    env->DeleteGlobalRef(taskClass);
}
