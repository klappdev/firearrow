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

#include "FileEraser.hpp"

#include <string>

#include "FileUnit.hpp"
#include <logging/Logging.hpp>
#include <util/strings/StringUtil.hpp>

namespace kl::fs {
    using fs::literals::operator""_kb;
    using fs::literals::operator""_gb;

    using namespace kl::util::strings;

    static constexpr const char* TAG = "FileEraser-JNI";

    FileEraser& FileEraser::instance() {
        static FileEraser eraser;
        return eraser;
    }

    Result<std::size_t, FileError> FileEraser::init(const std::filesystem::path& newPath, OverwriteMode newMode) {
        this->path = newPath;

        std::string errorMessage;

        if (std::filesystem::exists(path)) {
            eraseEntry.fileName = path;
        } else {
            errorMessage = "File doesn't exist";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        if (!std::filesystem::is_regular_file(path)) {
            errorMessage = "File isn't regular file or symlink";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        if (auto result = blockSize(path); result.hasValue()) {
            eraseEntry.bufferSize = result.value();
        } else {
            std::string& message = result.error().message;
            log::error(TAG, message);
            return static_cast<FileError>(result.error());
        }

        if (auto result = countHardLinks(path); result.hasError()) {
            errorMessage = "Count hard link must only be one";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        eraseEntry.fileSize = std::filesystem::file_size(path);
        eraseEntry.mode = newMode;

        return 0;
    }

    Result<std::size_t, FileError> FileEraser::checkPermission() {
        std::filesystem::perms permission = std::filesystem::status(path).permissions();
        showPermission(permission);

        std::error_code errorCode = {};

        std::filesystem::perms rwPermissions = std::filesystem::perms::owner_read | std::filesystem::perms::owner_write;
        std::filesystem::permissions(path, rwPermissions, std::filesystem::perm_options::add, errorCode);

        if (errorCode) {
            return FileError(errorCode.message());
        }

        return 0;
    }

    void FileEraser::showPermission(std::filesystem::perms permission) {
        log::debug(TAG, "owner permission: %s-%s-%s",
              ((permission & std::filesystem::perms::owner_read) != std::filesystem::perms::none ? "r" : "-"),
              ((permission & std::filesystem::perms::owner_write) != std::filesystem::perms::none ? "w" : "-"),
              ((permission & std::filesystem::perms::owner_exec) != std::filesystem::perms::none ? "x" : "-"));

        log::debug("group permission: %s-%s-%s",
              ((permission & std::filesystem::perms::group_read) != std::filesystem::perms::none ? "r" : "-"),
              ((permission & std::filesystem::perms::group_write) != std::filesystem::perms::none ? "w" : "-"),
              ((permission & std::filesystem::perms::group_exec) != std::filesystem::perms::none ? "x" : "-"));

        log::debug("other permission: %s-%s-%s",
              ((permission & std::filesystem::perms::others_read) != std::filesystem::perms::none ? "r" : "-"),
              ((permission & std::filesystem::perms::others_write) != std::filesystem::perms::none ? "w" : "-"),
              ((permission & std::filesystem::perms::others_exec) != std::filesystem::perms::none ? "x" : "-"));
    }

    Result<std::size_t, FileError> FileEraser::removeFile() {
        std::string parentPath = path.parent_path();
        std::string fileName = path.filename();
        std::filesystem::path copyPath = path;
        std::error_code errorCode = {};

        fileName = randomBuffer(fileName.size());
        copyPath.replace_filename(path);

        std::filesystem::rename(path, copyPath, errorCode);

        if (errorCode) {
            return FileError(errorCode.message());
        }

        std::filesystem::remove(copyPath, errorCode);

        if (errorCode) {
            return FileError(errorCode.message());
        }

        log::debug(TAG, "Remove file: %s", fileName.c_str());

        return 0;
    }

    Result<std::size_t, FileError> FileEraser::truncateFile(std::size_t size) {
        std::error_code errorCode = {};

        std::filesystem::resize_file(path, size, errorCode);

        if (errorCode) {
            return FileError(errorCode.message());
        }

        log::debug(TAG, "Truncate file: %s", eraseEntry.fileName.c_str());

        return 0;
    }

    Result<std::size_t, FileError> FileEraser::overwriteFile() {
        switch (eraseEntry.mode) {
        case OverwriteMode::SIMPLE_MODE:
            return overwriteByte(1, 0x00);
        case OverwriteMode::DOE_MODE:
            if (auto result = overwriteRandom(1); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteRandom(2); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteBytes(3, "DoE"); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            break;
        case OverwriteMode::OPENBSD_MODE:
            if (auto result = overwriteByte(1, 0xFF); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(2, 0x00); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(3, 0xFF); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            break;
        case OverwriteMode::RCMP_MODE:
            if (auto result = overwriteByte(1, 0x00); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(2, 0xFF); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteBytes(3, "RCMP"); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            break;
        case OverwriteMode::DOD_MODE:
            if (auto result = overwriteByte(1, 0xF6); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(2, 0x00); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(3, 0xFF); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteRandom(4); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(5, 0x00); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteByte(6, 0xFF); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            if (auto result = overwriteRandom(7); result.hasError()) {
                return static_cast<FileError>(result.error());
            }
            break;
        }

        return 0;
    }

    Result<std::size_t, FileError> FileEraser::overwriteByte(int pass, std::uint8_t byte) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;

        this->buffer = std::make_unique<uint8_t[]>(bufferSize);
        std::memset(buffer.get(), byte, bufferSize);
#if 0
        for (std::size_t i = 0; i < bufferSize; ++i) {
            log::info(TAG, "buffer[%d] = %d", i, uint32_t(buffer[i]));
        }
#endif
        if (auto result = makeOpenFile(fileName, "r+b"); result.hasValue()) {
            this->file = std::move(result.value());
        } else {
            return static_cast<FileError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FileError> FileEraser::overwriteBytes(int pass, const std::string& mask) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;

        buffer = std::make_unique<uint8_t[]>(bufferSize);
        maskBuffer(mask);
#if 0
        for (std::size_t i = 0; i < bufferSize; ++i) {
            log::info(TAG, "buffer[%d] = %d", i, uint32_t(buffer[i]));
        }
#endif
        if (auto result = makeOpenFile(fileName, "r+b"); result.hasValue()) {
            this->file = std::move(result.value());
        } else {
            return static_cast<FileError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FileError> FileEraser::overwriteRandom(int pass) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;

        std::string randomData = randomBuffer(bufferSize);
        buffer = std::make_unique<uint8_t[]>(bufferSize);

        std::copy(randomData.begin(), randomData.end(), buffer.get());
#if 0
        for (std::size_t i = 0; i < bufferSize; ++i) {
            log::info(TAG, "buffer[%d] = %d", i, uint32_t(buffer[i]));
        }
#endif
        if (auto result = makeOpenFile(fileName, "r+b"); result.hasValue()) {
            this->file = std::move(result.value());
        } else {
            return static_cast<FileError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FileError> FileEraser::overwriteBuffer(int pass) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;

        const size_t count = fileSize / bufferSize;
        const size_t tail  = fileSize % bufferSize;

        std::size_t written = 0;
        std::string errorMessage;

        log::debug(TAG, "Overwrite [buffer size=%d, file size=%lu, count=%zu, tail=%zu, pass=%d]",
                   bufferSize, fileSize, count, tail, pass);

        if (::fseek(file.get(), 0, SEEK_SET) != 0) {
            errorMessage = "Fail seek in file";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        if (auto result = writeBuffer(count, tail); result.hasValue()) {
            written = result.value();
        } else {
            return static_cast<FileError>(result.error());
        }

        if (written != fileSize) {
            errorMessage = "Fail overwrite all file";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        ::fflush(file.get());

        if (::fseek(file.get(), 0, SEEK_SET) != 0) {
            errorMessage = "Fail seek in file";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        file.reset();

        return written;
    }

    std::size_t FileEraser::maskBuffer(const std::string& mask) {
        for (size_t j = 0, i = eraseEntry.bufferSize - 1; i >= 0; i--) {
            buffer[i] = mask[j];

            if (++j == mask.size()) {
                j = 0;
            }
        }

        return mask.size();
    }

    Result<std::size_t, FileError> FileEraser::writeBuffer(std::size_t count, std::size_t tail) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;
        std::size_t written = 0;
        std::string errorMessage;

        if (count != 0) {
            for (std::size_t i = 0; i < count; ++i) {
                if (auto tmp = std::fwrite(buffer.get(), 1, bufferSize, file.get()); tmp == bufferSize) {
                    written += tmp;
                } else {
                    errorMessage = "Fail write buffer to file";
                    log::error(TAG, errorMessage);
                    return FileError(errorMessage);
                }
            }
        }

        if (auto tmp = std::fwrite(buffer.get(), 1, tail, file.get()); tmp == tail) {
            written += tmp;
        } else {
            errorMessage = "Fail write tail buffer to file";
            log::error(TAG, errorMessage);
            return FileError(errorMessage);
        }

        log::debug(TAG, "File written %lu with size %lu", written, fileSize);

        return written;
    }
}
