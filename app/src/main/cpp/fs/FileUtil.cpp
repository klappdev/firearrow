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

#include "FileUtil.hpp"

#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>

namespace kl::fs {

    void FileDeleter::operator()(FILE *fd)  {
        if (fd != nullptr) {
            ::fclose(fd);
        }
    }

    Result<FileUniquePtr, FileError> makeOpenFile(const std::string& path, const std::string& mode) {
        FILE *handle = ::fopen(path.c_str(), mode.c_str());

        if (handle == nullptr) {
            return FileError("Can't open file %s, error %s", path.c_str(), ::strerror(errno));
        }

        return FileUniquePtr(handle);
    }

    Result<std::size_t, FileError> blockSize(const std::string& path) {
        struct stat fileInfo = {};

        if (::lstat(path.c_str(), &fileInfo) == -1) {
            return FileError("Can't get file %s status , error %s", path.c_str(), ::strerror(errno));
        }

        std::size_t size = fileInfo.st_blksize;

        return size > 16 ? size : 512;
    }

    Result<std::uint32_t, FileError> countHardLinks(const std::string& path) {
        struct stat fileInfo = {};

        if (::lstat(path.c_str(), &fileInfo) < 0) {
            return FileError("Can't get file %s status, error %s", path.c_str(), ::strerror(errno));
        }

        return static_cast<std::uint32_t>(fileInfo.st_nlink);
    }
}