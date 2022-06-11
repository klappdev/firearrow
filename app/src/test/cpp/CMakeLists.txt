#
# Tests are executed only once during build.
#
# Tests are run on the targed device, and will fail, if device is not connected.
# If require to build project without installing, tests should be disabled.
#
# Tests are enabled by default.
# To disable tests add line "enableCppTests=false" to the file "local.properties".
#

if (${ENABLE_CPP_TESTS} MATCHES "true")

    # include gtest library
    set(GTEST_DIR ${ANDROID_NDK}/sources/third_party/googletest)
    add_library(gtest STATIC ${GTEST_DIR}/src/gtest_main.cc ${GTEST_DIR}/src/gtest-all.cc)
    target_include_directories(gtest PRIVATE ${GTEST_DIR})
    target_include_directories(gtest PUBLIC ${GTEST_DIR}/include)

    # include test source files
    add_executable(firearrowTest
            ${TEST_SRC_DIR}/EnumerationTest.cpp
            ${TEST_SRC_DIR}/PropertyTest.cpp
            ${TEST_SRC_DIR}/NullabilityTest.cpp
    )

    target_link_libraries(firearrowTest firearrow gtest)

    # prepare test variables
    set(TARGET_TEST_DIR /data/local/tmp/firearrowTest)
    set(TARGET_TEST_LIB_DIR ${TARGET_TEST_DIR}/${ANDROID_ABI})
    message("ANDROID_SDK_ROOT: ${ANDROID_SDK_ROOT}")

    # search adb binary
    find_program(ADB NAMES adb PATHS ${ANDROID_SDK_ROOT}/platform-tools)

    add_custom_command(TARGET firearrowTest POST_BUILD
            COMMAND ${ADB} shell mkdir -p ${TARGET_TEST_LIB_DIR}

            # Push binary and library
            COMMAND ${ADB} push $<TARGET_FILE:firearrowTest> ${TARGET_TEST_LIB_DIR}/
            COMMAND ${ADB} push $<TARGET_FILE:firearrow> ${TARGET_TEST_LIB_DIR}/

            # Execute tests
            COMMAND ${ADB} shell \"export LD_LIBRARY_PATH=${TARGET_TEST_LIB_DIR}/\; ${TARGET_TEST_LIB_DIR}/firearrowTest\"
    )
endif ()