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

#include "FsEraser.hpp"

#include <string>

#include "FsUnit.hpp"
#include "../logging/Logging.hpp"
#include "../util/functional/Match.hpp"
#include "../util/strings/StringUtil.hpp"

namespace kl::fs {
    using fs::literals::operator""_kb;
    using fs::literals::operator""_gb;

    using namespace util::strings;

    static constexpr const char* TAG = "FsEraser-JNI";

    Result<std::size_t, FsError> FsEraser::init(const std::filesystem::path& newPath, OverwriteMode newMode) {
        this->path = newPath;

        std::string errorMessage;

        if (std::filesystem::exists(path)) {
            eraseEntry.fileName = path;
        } else {
            errorMessage = "File doesn't exist";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        if (!std::filesystem::is_regular_file(path)) {
            errorMessage = "File isn't regular file or symlink";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        if (auto fileSize = std::filesystem::file_size(path); fileSize >= 0 || fileSize < 2_gb) {
            eraseEntry.fileSize = fileSize;
        } else {
            errorMessage = "File size can't be negative or more 2GB";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        if (auto result = blockSize(path); result.hasValue()) {
            eraseEntry.bufferSize = result.value();
        } else {
            std::string& message = result.error().message;
            log::error(TAG, message);
            return static_cast<FsError>(result.error());
        }

        if (auto result = countHardLinks(path); result.hasError()) {
            errorMessage = "Count hard link must only be one";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        eraseEntry.mode = newMode;

        return 0;
    }

    Result<std::size_t, FsError> FsEraser::checkPermission() {
        std::filesystem::perms permission = std::filesystem::status(path).permissions();
        showPermission(permission);

        std::error_code errorCode = {};

        std::filesystem::perms rwPermissions = std::filesystem::perms::owner_read | std::filesystem::perms::owner_write;
        std::filesystem::permissions(path, rwPermissions, std::filesystem::perm_options::add, errorCode);

        if (errorCode) {
            return FsError(errorCode.message());
        }

        return 0;
    }

    void FsEraser::showPermission(std::filesystem::perms permission) {
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

    Result<std::size_t, FsError> FsEraser::removeFile() {
        std::string parentPath = path.parent_path();
        std::string fileName = path.filename();
        std::filesystem::path copyPath = path;
        std::error_code errorCode = {};

        fileName = random(fileName.size());
        copyPath.replace_filename(path);

        std::filesystem::rename(path, copyPath, errorCode);

        if (errorCode) {
            return FsError(errorCode.message());
        }

        std::filesystem::remove(copyPath, errorCode);

        if (errorCode) {
            return FsError(errorCode.message());
        }

        log::debug(TAG, "Remove file: %s", fileName.c_str());

        return 0;
    }

    Result<std::size_t, FsError> FsEraser::truncateFile(size_t size) {
        std::error_code errorCode = {};

        std::filesystem::resize_file(path, size, errorCode);

        if (errorCode) {
            return FsError(errorCode.message());
        }

        log::debug(TAG, "Truncate file: %s", eraseEntry.fileName.c_str());

        return 0;
    }

    Result<std::size_t, FsError> FsEraser::overwriteFile() {
        switch (eraseEntry.mode) {
        case OverwriteMode::SIMPLE_MODE:
            return overwriteByte(1, 0x00);
        case OverwriteMode::DOE_MODE:
            if (auto result = overwriteRandom(1); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(2); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(3, "DoE"); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            break;
        case OverwriteMode::OPENBSD_MODE:
            if (auto result = overwriteByte(1, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(2, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(3, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            break;
        case OverwriteMode::RCMP_MODE:
            if (auto result = overwriteByte(1, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(2, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(3, "RCMP"); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            break;
        case OverwriteMode::DOD_MODE:
            if (auto result = overwriteByte(1, 0xF6); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(2, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(3, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(4); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(5, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(6, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(7); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            break;
        case OverwriteMode::GUTMAN_MODE:
            if (auto result = overwriteRandom(1); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(2); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(3); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(4); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(5, 0x55); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(6, 0xAA); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(7, "’I$"); result.hasError()) { /* 0x92, 0x49, 0x24 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(8, "I$’"); result.hasError()) { /* 0x49, 0x24, 0x92 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(9, "$’I"); result.hasError()) { /* 0x24, 0x92, 0x49 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(10, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(11, 0x11); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(12, 0x22); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(13, 0x33); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(14, 0x44); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(15, 0x55); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(16, 0x66); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(17, 0x77); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(18, 0x88); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(19, 0x99); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(20, 0xAA); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(21, 0xBB); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(22, 0xCC); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(23, 0xDD); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(24, 0xEE); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(25, 0xFF); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(26, "’I$"); result.hasError()) { /* 0x92, 0x49, 0x24 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(27, "I$’"); result.hasError()) { /* 0x49, 0x24, 0x92 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(28, "$’I"); result.hasError()) { /* 0x24, 0x92, 0x49 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(29, "m¶Û"); result.hasError()) { /* 0x6D, 0xB6, 0xDB */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(30, "¶Ûm"); result.hasError()) { /* 0xB6, 0xDB, 0x6D */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteBytes(31, "Ûm¶"); result.hasError()) { /* 0xDB, 0x6D, 0xB6 */
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(32); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(33); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(34); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteRandom(35); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            if (auto result = overwriteByte(36, 0x00); result.hasError()) {
                return static_cast<FsError>(result.error());
            }
            break;
        }

        return 0;
    }

    Result<std::size_t, FsError> FsEraser::overwriteByte(int pass, std::uint8_t byte) {
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
            return static_cast<FsError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FsError> FsEraser::overwriteBytes(int pass, const std::string& mask) {
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
            return static_cast<FsError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FsError> FsEraser::overwriteRandom(int pass) {
        const auto& [fileName, fileSize, bufferSize, mode] = eraseEntry;

        std::string randomData = random(bufferSize);
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
            return static_cast<FsError>(result.error());
        }

        return overwriteBuffer(pass);
    }

    Result<std::size_t, FsError> FsEraser::overwriteBuffer(int pass) {
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
            return FsError(errorMessage);
        }

        if (auto result = writeBuffer(count, tail); result.hasValue()) {
            written = result.value();
        } else {
            return static_cast<FsError>(result.error());
        }

        if (written != fileSize) {
            errorMessage = "Fail overwrite all file";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        ::fflush(file.get());

        if (::fseek(file.get(), 0, SEEK_SET) != 0) {
            errorMessage = "Fail seek in file";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        file.reset();

        return written;
    }

    std::size_t FsEraser::maskBuffer(const std::string& mask) {
        for (size_t j = 0, i = eraseEntry.bufferSize - 1; i >= 0; i--) {
            buffer[i] = mask[j];

            if (++j == mask.size()) {
                j = 0;
            }
        }

        return mask.size();
    }

    Result<std::size_t, FsError> FsEraser::writeBuffer(std::size_t count, std::size_t tail) {
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
                    return FsError(errorMessage);
                }
            }
        }

        if (auto tmp = std::fwrite(buffer.get(), 1, tail, file.get()); tmp == tail) {
            written += tmp;
        } else {
            errorMessage = "Fail write tail buffer to file";
            log::error(TAG, errorMessage);
            return FsError(errorMessage);
        }

        log::debug(TAG, "File written %lu with size %lu", written, fileSize);

        return written;
    }
}
