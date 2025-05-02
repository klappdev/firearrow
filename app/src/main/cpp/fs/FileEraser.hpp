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
#pragma once

#include <filesystem>
#include <memory>

#include "EraseEntry.hpp"
#include "OverwriteMode.hpp"
#include "FileUtil.hpp"
#include "FileError.hpp"

#include <error/Result.hpp>

namespace firearrow::fs {
    using namespace error;

    class FileEraser final {
    public:
        FileEraser() = default;
        ~FileEraser() = default;

        Result<void, FileError> init(const std::filesystem::path& path, OverwriteMode newMode);

        Result<void, FileError> checkPermission();
        Result<void, FileError> removeFile();
        Result<void, FileError> overwriteFile();
        Result<void, FileError> truncateFile(std::size_t size);

    private:
        void showPermission(std::filesystem::perms permission);

        Result<std::size_t, FileError> overwriteByte(int pass, std::uint8_t symbol);
        Result<std::size_t, FileError> overwriteRandom(int pass);
        Result<std::size_t, FileError> overwriteBuffer(int pass);
        Result<std::size_t, FileError> writeBuffer(std::size_t count, std::size_t tail);

    private:
        std::unique_ptr<std::uint8_t[]> buffer;
        EraseEntry eraseEntry;

        std::filesystem::path path;
        FileUniquePtr file;
    };
}