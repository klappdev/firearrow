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

#include "Logging.hpp"

#include <android/log.h>

namespace kl::log {

    void info(const char* tag, const char* format, ...) {
        va_list arguments;
        va_start(arguments, format);
        __android_log_vprint(ANDROID_LOG_INFO, tag, format, arguments);
        va_end(arguments);
    }

    void info(const char* tag, const std::string& message) {
        __android_log_write(ANDROID_LOG_INFO, tag, message.c_str());
    }

    void debug(const char* tag, const char* format, ...) {
        va_list arguments;
        va_start(arguments, format);
        __android_log_vprint(ANDROID_LOG_DEBUG, tag, format, arguments);
        va_end(arguments);
    }

    void debug(const char* tag, const std::string& message) {
        __android_log_write(ANDROID_LOG_DEBUG, tag, message.c_str());
    }

    void error(const char* tag, const char* format, ...) {
        va_list arguments;
        va_start(arguments, format);
        __android_log_vprint(ANDROID_LOG_ERROR, tag, format, arguments);
        va_end(arguments);
    }

    void error(const char* tag, const std::string& message) {
        __android_log_write(ANDROID_LOG_DEBUG, tag, message.c_str());
    }
}

