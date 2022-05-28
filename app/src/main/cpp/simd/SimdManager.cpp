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

#include <vector>
#include <experimental/simd>

#include "SimdAbi.hpp"
#include <jni/UniqueUtfChars.hpp>
#include <util/nullability/NonNull.hpp>
#include <util/nullability/Nullable.hpp>

namespace {
    jclass simdResultClass = nullptr;
    jclass simdAbiClass = nullptr;

    jclass integerClass = nullptr;
    jclass integerArrayClass = nullptr;
    jclass illegalArgumentExceptionClass = nullptr;

    jmethodID simdResultConstructorId = nullptr;
    jmethodID integerConstructorId = nullptr;

    jmethodID getCountBytesMethodId = nullptr;
    jmethodID intValueMethodId = nullptr;
}

using namespace kl::util::nullability;

namespace kl::simd {

    std::vector<std::int32_t> convertFromJvmArray(const NonNull<JNIEnv*>& env, jobjectArray jvmArray,
                                                  std::size_t lengthArray) {
        std::vector<std::int32_t> nativeArray = {};

        for (std::int32_t i = 0; i < lengthArray; ++i) {
            jobject element = env->GetObjectArrayElement(jvmArray, i);

            nativeArray[i] = env->CallIntMethod(element, intValueMethodId);
        }

        return nativeArray;
    }

    jobjectArray convertToJvmArray(const NonNull<JNIEnv*>& env,
                                   const std::vector<std::int32_t>& nativeArray) {
        jobjectArray jvmArray = env->NewObjectArray(static_cast<jsize>(nativeArray.size()), integerClass, nullptr);

        for (std::int32_t i = 0; i < nativeArray.size(); ++i) {
            jobject element = env->NewObject(integerClass, integerConstructorId, nativeArray[i]);
            env->SetObjectArrayElement(jvmArray, i, element);
        }

        return jvmArray;
    }

    std::vector<std::int32_t> sumScalarArray(const std::vector<std::int32_t>& leftArray,
                                             const std::vector<std::int32_t>& rightArray) {
        std::vector<std::int32_t> resultArray;
        resultArray.reserve(leftArray.size());

        for (std::size_t i = 0; i < leftArray.size(); ++i) {
            resultArray[i] = leftArray[i] + rightArray[i];
        }

        return resultArray;
    }

    template<std::size_t N>
    std::vector<std::int32_t> sumSimdArray(const std::vector<std::int32_t>& leftArray,
                                           const std::vector<std::int32_t>& rightArray) {
        std::vector<std::int32_t> resultArray = {};
#if __cpp_lib_experimental_parallel_simd
        constexpr std::size_t simdSize = (N / sizeof(std::int32_t));

        std::experimental::fixed_size_simd<std::int32_t, simdSize> leftVector;
        std::experimental::fixed_size_simd<std::int32_t, simdSize> rightVector;

        for (std::size_t i = 0; i < leftArray.size() / simdSize; ++i) {
            leftVector.copy_from(&leftArray[i * simdSize], std::experimental::element_aligned);
            rightVector.copy_from(&rightArray[i * simdSize], std::experimental::element_aligned);

            auto resultVector = leftVector + rightVector;
            resultVector.copy_to(&resultArray[i * simdSize], std::experimental::element_aligned);
        }
#else
        resultArray = sumScalarArray(leftArray, rightArray);
#endif

        return resultArray;
    }

    jboolean nativeIsSupported(JNIEnv* rawEnv, jclass clazz, jobject jvmSimdAbi) {
        /*FIXME: implement in future*/
        return JNI_FALSE;
    }

    jobject nativeSumScalarArrays(JNIEnv* rawEnv, jclass clazz,
                                  jobjectArray jvmLeftArray, jobjectArray jvmRightArray) {
        auto env = makeNonNull(rawEnv);
        std::vector<std::int32_t> nativeArray;

        if (!env->IsInstanceOf(jvmLeftArray, integerArrayClass)) {
            env->ThrowNew(illegalArgumentExceptionClass, "Native simd support integer array");
            return nullptr;
        }

        std::size_t leftArraySize = env->GetArrayLength(jvmLeftArray);
        std::size_t rightArraySize = env->GetArrayLength(jvmRightArray);

        if (leftArraySize != rightArraySize) {
            env->ThrowNew(illegalArgumentExceptionClass, "Native scalar arrays must be same size");
            return nullptr;
        }

        auto leftArray = convertFromJvmArray(env, jvmLeftArray, leftArraySize);
        auto rightArray = convertFromJvmArray(env, jvmRightArray, leftArraySize);

        auto beginTime = std::chrono::steady_clock::now();

        nativeArray = sumScalarArray(leftArray, rightArray);

        auto endTime = std::chrono::steady_clock::now();

        return env->NewObject(simdResultClass, simdResultConstructorId,
                convertToJvmArray(env, nativeArray), JNI_FALSE,
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
    }

    jobject nativeSumSimdArrays(JNIEnv* rawEnv, jclass clazz, jobjectArray jvmLeftArray,
                            jobjectArray jvmRightArray, jobject jvmSimdAbi) {
        auto env = makeNonNull(rawEnv);
        std::vector<std::int32_t> nativeArray;

        if (!env->IsInstanceOf(jvmLeftArray, integerArrayClass)) {
            env->ThrowNew(illegalArgumentExceptionClass, "Native simd support integer array");
            return nullptr;
        }

        jint jvmCountBytes = env->CallIntMethod(jvmSimdAbi, getCountBytesMethodId);
        std::size_t leftArraySize = env->GetArrayLength(jvmLeftArray);
        std::size_t rightArraySize = env->GetArrayLength(jvmRightArray);

        if (leftArraySize != jvmCountBytes || rightArraySize != jvmCountBytes) {
            env->ThrowNew(illegalArgumentExceptionClass, "Native scalar arrays must be same size");
            return nullptr;
        }

        auto leftArray = convertFromJvmArray(env, jvmLeftArray, leftArraySize);
        auto rightArray = convertFromJvmArray(env, jvmRightArray, leftArraySize);

        auto beginTime = std::chrono::steady_clock::now();

        switch (jvmCountBytes) {
        case SIMD_ABI.ordinal(SimdAbi::SIMD_128_BITS): {
            nativeArray = sumSimdArray<SIMD_ABI.ordinal(SimdAbi::SIMD_128_BITS)>(leftArray, rightArray);
            break;
        }
        case SIMD_ABI.ordinal(SimdAbi::SIMD_256_BITS): {
            nativeArray = sumSimdArray<SIMD_ABI.ordinal(SimdAbi::SIMD_256_BITS)>(leftArray, rightArray);
            break;
        }
        case SIMD_ABI.ordinal(SimdAbi::SIMD_512_BITS): {
            nativeArray = sumSimdArray<SIMD_ABI.ordinal(SimdAbi::SIMD_512_BITS)>(leftArray, rightArray);
            break;
        }
        default: break;
        }

        auto endTime = std::chrono::steady_clock::now();

        return env->NewObject(simdResultClass, simdResultConstructorId,
                convertToJvmArray(env, nativeArray), nativeIsSupported(rawEnv, clazz, jvmSimdAbi),
                std::chrono::duration_cast<std::chrono::milliseconds>(endTime - beginTime).count());
    }

    jobject nativeSumSimdMaskArrays(JNIEnv* rawEnv, jclass clazz, jobjectArray jvmLeftArray,
                                    jobjectArray jvmRightArray, jobject jvmMask, jobject jvmSimdAbi) {
        /*FIXME: implement in future*/
        return nullptr;
    }

    constexpr std::array<JNINativeMethod, 4> JNI_METHODS = {{
        {"isSupported", "(Lorg/kl/firearrow/simd/SimdAbi;)Z", (void*)nativeIsSupported},
        {"sumArrays",
         "([Ljava/lang/Number;[Ljava/lang/Number;)Lorg/kl/firearrow/simd/SimdResult;",
         (void*)nativeSumScalarArrays},
        {"sumArrays",
         "([Ljava/lang/Number;[Ljava/lang/Number;Lorg/kl/firearrow/simd/SimdAbi;)Lorg/kl/firearrow/simd/SimdResult;",
         (void*)nativeSumSimdArrays},
        {"sumArrays",
         "([Ljava/lang/Number;[Ljava/lang/Number;Ljava/util/BitSet;Lorg/kl/firearrow/simd/SimdAbi;)Lorg/kl/firearrow/simd/SimdResult;",
         (void*)nativeSumSimdMaskArrays},
    }};
}

jint registerSimdManager(JNIEnv* rawEnv) {
    using kl::simd::JNI_METHODS;
    auto env = makeNonNull(rawEnv);

    jclass temporaryClass = env->FindClass("java/lang/Integer");
    integerClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("[Ljava/lang/Integer;");
    integerArrayClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("java/lang/IllegalArgumentException");
    illegalArgumentExceptionClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/simd/SimdResult");
    simdResultClass = (jclass) env->NewGlobalRef(temporaryClass);

    temporaryClass = env->FindClass("org/kl/firearrow/simd/SimdAbi");
    simdAbiClass = (jclass) env->NewGlobalRef(temporaryClass);

    simdResultConstructorId = env->GetMethodID(simdResultClass, "<init>", "([Ljava/lang/Number;ZJ)V");
    integerConstructorId = env->GetMethodID(integerClass, "<init>", "(I)V");

    getCountBytesMethodId = env->GetMethodID(simdAbiClass, "getCountBytes", "()I");
    intValueMethodId = env->GetMethodID(integerClass, "intValue", "()I");

    jclass simdManagerClass = env->FindClass("org/kl/firearrow/simd/SimdManager");
    return env->RegisterNatives(simdManagerClass, JNI_METHODS.data(), JNI_METHODS.size());
}

void unregisterSimdManager(JNIEnv* rawEnv) {
    auto env = makeNonNull(rawEnv);

    env->DeleteGlobalRef(simdResultClass);
    env->DeleteGlobalRef(simdAbiClass);
    env->DeleteGlobalRef(integerClass);
    env->DeleteGlobalRef(integerArrayClass);
}

