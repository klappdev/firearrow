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
#pragma once

#if __has_include(<filesystem>)
#   include <filesystem>
#else
#   error "missing header <filesystem>"
#endif

#include <memory>

#include "EraseEntry.hpp"
#include "OverwriteMode.hpp"
#include "FsUtil.hpp"
#include "FsError.hpp"

#include <util/property/Property.hpp>
#include <util/functional/Result.hpp>

namespace kl::fs {
    using namespace util::functional;

    class FsEraser final {
    public:
        FsEraser() = default;
        ~FsEraser() = default;

        Result<std::size_t, FsError> init(const std::filesystem::path& path, OverwriteMode newMode);

        Result<std::size_t, FsError> checkPermission();
        Result<std::size_t, FsError> removeFile();
        Result<std::size_t, FsError> overwriteFile();
        Result<std::size_t, FsError> truncateFile(size_t size);

    private:
        void showPermission(std::filesystem::perms permission);

        Result<std::size_t, FsError> overwriteByte(int pass, std::uint8_t symbol);
        Result<std::size_t, FsError> overwriteBytes(int pass, const std::string& mask);
        Result<std::size_t, FsError> overwriteRandom(int pass);
        Result<std::size_t, FsError> overwriteBuffer(int pass);

        std::size_t maskBuffer(const std::string& mask);
        Result<std::size_t, FsError> writeBuffer(std::size_t count, std::size_t tail);

    private:
        std::unique_ptr<std::uint8_t[]> buffer;
        EraseEntry eraseEntry;

        std::filesystem::path path;
        FileUniquePtr file;
    };
}