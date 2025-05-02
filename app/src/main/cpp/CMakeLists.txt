cmake_minimum_required(VERSION 3.18.1)
project("firearrow")

# include main directory
include_directories(.)

set(CMAKE_CXX_STANDARD 20)

# coroutines support
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCES
    core/LoadLibrary.cpp
    coroutine/CoroutineManager.cpp

    backtrace/BacktraceFrame.cpp
    backtrace/Backtrace.cpp

    fs/FileManager.cpp
    fs/FileEraser.cpp
    fs/FileUtil.cpp

    simd/SimdManager.cpp
    net/NetworkManager.cpp
    net/HttpClient.cpp

    jni/UniqueUtfChars.cpp
    jni/UniqueJniEnv.cpp
    jni/JniException.cpp

    logging/Logging.cpp
    strings/StringUtil.cpp
    time/StopWatch.cpp
)

# neon
if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
    set_property(SOURCE ${SOURCES}
        APPEND_STRING PROPERTY COMPILE_FLAGS " -mfpu=neon"
    )
    add_definitions("-DHAVE_NEON=1")
elseif (${ANDROID_ABI} STREQUAL "x86")
    set_property(SOURCE ${SOURCES}
        APPEND_STRING PROPERTY COMPILE_FLAGS
        " -mfloat-abi=hard -mssse3  -Wno-unknown-attributes \
          -Wno-deprecated-declarations \
          -Wno-constant-conversion \
          -Wno-static-in-inline"
    )
    add_definitions(-DHAVE_NEON_X86=1 -DHAVE_NEON=1)
elseif (${ANDROID_ABI} STREQUAL "x86_64")
    set_property(SOURCE ${SOURCES}
        APPEND_STRING PROPERTY COMPILE_FLAGS " -mfloat-abi=hard"
    )
    add_definitions(-DHAVE_NEON_X86=1 -DHAVE_NEON=1)
else ()
#    set_property(SOURCE ${SOURCES}
#        APPEND_STRING PROPERTY COMPILE_FLAGS " -mfloat-abi=hard"
#    )
    add_definitions("-DHAVE_NEON=1")
endif ()

# include source files
add_library(firearrow SHARED
    ${SOURCES}
)

set(CLANG_VERSION 19)

# libunwind.a
add_library(unwind STATIC IMPORTED)
if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(LIBUNWIND_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/${CLANG_VERSION}/lib/linux/aarch64/libunwind.a)
else()
    set(LIBUNWIND_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/lib/clang/${CLANG_VERSION}/lib/linux/arm/libunwind.a)
endif()
set_target_properties(unwind PROPERTIES IMPORTED_LOCATION ${LIBUNWIND_PATH})
include_directories(${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/clang/${CLANG_VERSION}/include)

# libc++abi.a
add_library(c++abi STATIC IMPORTED)
if (${CMAKE_ANDROID_ARCH_ABI} STREQUAL "arm64-v8a")
    set(LIBC++ABI_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/libc++abi.a)
else()
    set(LIBC++ABI_PATH ${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/arm-linux-androideabi/libc++abi.a)
endif()
set_target_properties(c++abi PROPERTIES IMPORTED_LOCATION ${LIBC++ABI_PATH})
include_directories(${CMAKE_ANDROID_NDK}/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/include/c++/v1)

# libcpu-features
add_library(cpufeatures STATIC
    ${CMAKE_ANDROID_NDK}/sources/android/cpufeatures/cpu-features.c
)
include_directories(${CMAKE_ANDROID_NDK}/sources/android/cpufeatures)

target_link_libraries(firearrow
    log
    unwind
    c++abi
    cpufeatures
)

#include tests configuration
set(TEST_SRC_DIR ../../test/cpp)
include(../../test/cpp/CMakeLists.txt)