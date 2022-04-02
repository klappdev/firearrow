cmake_minimum_required(VERSION 3.18.1)
project("firearrow")

set(CMAKE_CXX_STANDARD 20)

# coroutines support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fcoroutines-ts -std=c++2a -Wall")

add_library(firearrow SHARED
        core/LoadLibrary.cpp

        backtrace/BacktraceFrame.cpp
        backtrace/Backtrace.cpp

        fs/FsManager.cpp
        fs/FsEraser.cpp
        fs/FsUtil.cpp

        jni/UniqueUtfChars.cpp
        jni/UniqueJniEnv.cpp
        jni/JniException.cpp

        logging/Logging.cpp
        util/strings/StringUtil.cpp
)

set(CLANG_VERSION 12.0.5)

# libunwind.a
add_library(unwind STATIC IMPORTED)
if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(LIBUNWIND_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/${CLANG_VERSION}/lib/linux/aarch64/libunwind.a)
else()
    set(LIBUNWIND_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/${CLANG_VERSION}/lib/linux/arm/libunwind.a)
endif()
set_target_properties(unwind PROPERTIES IMPORTED_LOCATION ${LIBUNWIND_PATH})
include_directories(${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib64/clang/${CLANG_VERSION}/include)

# libc++abi.a
add_library(c++abi STATIC IMPORTED)
set(LIBC++ABI_PATH ${CMAKE_ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/${CMAKE_ANDROID_ARCH_ABI}/libc++abi.a)
set_target_properties(c++abi PROPERTIES IMPORTED_LOCATION ${LIBC++ABI_PATH})
include_directories(${CMAKE_ANDROID_NDK}/sources/cxx-stl/llvm-libc++abi/include)

target_link_libraries(firearrow
        log
        unwind
        c++abi
)