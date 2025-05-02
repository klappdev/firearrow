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

#include <jni.h>
#include <cpu-features.h>

#include "SimdArithmetic.hpp"

#include <nullability/NonNull.hpp>
#include <nullability/Nullable.hpp>
#include <time/StopWatch.hpp>

namespace {
    jclass simdResultClass = nullptr;
    jclass simdAbiClass = nullptr;
    jclass simdExceptionClass = nullptr;

    jmethodID simdResultConstructorId = nullptr;
}

namespace firearrow::simd {
    using namespace nullability;
    using namespace time;

    std::vector<std::int32_t> convertFromJvmArray(const NonNull<JNIEnv*>& env, jintArray jvmArray, std::size_t lengthArray) {
        jint* jvmRawArray = env->GetIntArrayElements(jvmArray, nullptr);

        std::vector<std::int32_t> nativeArray(jvmRawArray, std::next(jvmRawArray, lengthArray));

        env->ReleaseIntArrayElements(jvmArray, jvmRawArray, JNI_ABORT);

        return nativeArray;
    }

    jintArray convertToJvmArray(const NonNull<JNIEnv*>& env, const std::vector<std::int32_t>& nativeArray) {
        jintArray jvmArray = env->NewIntArray(nativeArray.size());

        env->SetIntArrayRegion(jvmArray, 0, nativeArray.size(), nativeArray.data());

        return jvmArray;
    }

    jboolean nativeIsSimdAbiSupported() {
        return android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON != 0;
    }

    template<SimdArithmeticType SAT>
    std::vector<std::int32_t> simdNeonOperationArrays(int32_t leftArray[], int32_t rightArray[], size_t arraySize) {
        std::vector<std::int32_t> targetArray(arraySize, 0);

        if (SAT == SimdArithmeticType::SIMD_ADDITION) {
            addNeonArray(leftArray, rightArray, &targetArray[0], targetArray.size());
        } else if (SAT == SimdArithmeticType::SIMD_SUBTRACT) {
            subNeonArray(leftArray, rightArray, &targetArray[0], targetArray.size());
        }

        return targetArray;
    }

    template<SimdArithmeticType SAT>
    jobject simdOperationArrays(JNIEnv* env, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        const std::size_t jvmLeftArraySize = env->GetArrayLength(jvmLeftArray);
        const std::size_t jvmRightArraySize = env->GetArrayLength(jvmRightArray);

        if (jvmLeftArraySize != jvmRightArraySize) {
            env->ThrowNew(simdExceptionClass, "Jvm scalar arrays must be same size");
            return nullptr;
        }

        auto leftArray = convertFromJvmArray(env, jvmLeftArray, jvmLeftArraySize);
        auto rightArray = convertFromJvmArray(env, jvmRightArray, jvmLeftArraySize);

        StopWatch stopWatch;
        stopWatch.start();

        const std::vector<std::int32_t> nativeArray = simdNeonOperationArrays<SAT>(leftArray.data(), rightArray.data(), jvmLeftArraySize);

        stopWatch.stop();

        return env->NewObject(simdResultClass, simdResultConstructorId, convertToJvmArray(env, nativeArray),
                              nativeIsSimdAbiSupported(), static_cast<jlong>(stopWatch.getDuration()));
    }

    template<SimdArithmeticType SAT>
    jintArray simdFastOperationArrays(JNIEnv* env, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        const std::size_t jvmLeftArraySize = env->GetArrayLength(jvmLeftArray);
        const std::size_t jvmRightArraySize = env->GetArrayLength(jvmRightArray);

        if (jvmLeftArraySize != jvmRightArraySize) {
            env->ThrowNew(simdExceptionClass, "Jvm scalar arrays must be same size");
            return nullptr;
        }

        jboolean isCopy;
        auto* leftArray = static_cast<jint*>(env->GetPrimitiveArrayCritical(jvmLeftArray, &isCopy));
        auto* rightArray = static_cast<jint*>(env->GetPrimitiveArrayCritical(jvmRightArray, &isCopy));

        const std::vector<std::int32_t> nativeArray = simdNeonOperationArrays<SAT>(leftArray, rightArray, jvmLeftArraySize);

        env->ReleasePrimitiveArrayCritical(jvmLeftArray, leftArray, JNI_ABORT);
        env->ReleasePrimitiveArrayCritical(jvmRightArray, rightArray, JNI_ABORT);

        return convertToJvmArray(env, nativeArray);
    }

    jobject nativeAddSimdArrays(JNIEnv* rawEnv, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        return simdOperationArrays<SimdArithmeticType::SIMD_ADDITION>(rawEnv, clazz, jvmLeftArray, jvmRightArray);
    }

    jintArray nativeFastAddSimdArrays(JNIEnv* rawEnv, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        return simdFastOperationArrays<SimdArithmeticType::SIMD_ADDITION>(rawEnv, clazz, jvmLeftArray, jvmRightArray);
    }

    jobject nativeSubSimdArrays(JNIEnv* rawEnv, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        return simdOperationArrays<SimdArithmeticType::SIMD_SUBTRACT>(rawEnv, clazz, jvmLeftArray, jvmRightArray);
    }

    jintArray nativeFastSubSimdArrays(JNIEnv* rawEnv, jclass clazz, jintArray jvmLeftArray, jintArray jvmRightArray) {
        return simdFastOperationArrays<SimdArithmeticType::SIMD_SUBTRACT>(rawEnv, clazz, jvmLeftArray, jvmRightArray);
    }

    constexpr std::array<JNINativeMethod, 5> JNI_METHODS = {{
        {"nativeIsSimdSupported", "()Z", (void*)nativeIsSimdAbiSupported},
        {"nativeSimdAddArrays",
         "([I[I)Lorg/kl/firearrow/simd/SimdResult;",
         (void*)nativeAddSimdArrays},
        {"nativeFastSimdAddArrays",
         "([I[I)[I",
         (void*)nativeFastAddSimdArrays},
        {"nativeSimdSubArrays",
         "([I[I)Lorg/kl/firearrow/simd/SimdResult;",
         (void*)nativeSubSimdArrays},
        {"nativeFastSimdSubArrays",
         "([I[I)[I",
         (void*)nativeFastSubSimdArrays},
    }};
}

jint registerSimdManager(JNIEnv* env) {
    using firearrow::simd::JNI_METHODS;

    jclass temporaryClass = env->FindClass("org/kl/firearrow/simd/SimdException");
    simdExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/simd/SimdResult");
    simdResultClass = (jclass) env->NewGlobalRef(temporaryClass);

    simdResultConstructorId = env->GetMethodID(simdResultClass, "<init>", "([IZJ)V");

    jclass simdManagerClass = env->FindClass("org/kl/firearrow/simd/SimdManager");

    env->DeleteLocalRef(temporaryClass);

    return env->RegisterNatives(simdManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterSimdManager(JNIEnv* env) {
    env->DeleteGlobalRef(simdResultClass);
    env->DeleteGlobalRef(simdAbiClass);
}