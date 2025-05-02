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

#include "HttpClient.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

static constexpr char REQUEST_TEMPLATE[] = "GET / HTTP/1.1\r\nHost: %s\r\n\r\n";
static constexpr int BUFFER_SIZE = 8192;

namespace firearrow::net {
    using namespace strings;

    HttpClient::HttpClient()
        : fd(-1)
        , port(0) {

    }

    HttpClient::~HttpClient() {
        disconnect();
    }

    Result<std::uint32_t, NetworkError> HttpClient::create(const std::string& address, std::uint16_t port) {
        this->address = address;
        this->port = port;

        request = format(REQUEST_TEMPLATE, address.c_str());

        fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd == -1) {
            return NetworkError("Can't create socket, error %s", ::strerror(errno));
        }

        return static_cast<std::int32_t>(fd);
    }

    Result<void, NetworkError> HttpClient::connect() {
        struct hostent* hostEntry = ::gethostbyname(address.c_str());

        if (hostEntry == nullptr) {
            return NetworkError("Can't get host by name, error", ::strerror(errno));
        }

        if (hostEntry->h_addr_list == nullptr || hostEntry->h_addr_list[0] == nullptr) {
            return NetworkError("Can't get address list, error %s", ::hstrerror(h_errno));
        }

        const char* inetAddress = ::inet_ntoa(*(struct in_addr*)(hostEntry->h_addr_list));

        if (inetAddress == nullptr) {
            return NetworkError("Can't get inne address, error %s", ::hstrerror(h_errno));
        }

        in_addr_t networkAddress = ::inet_addr(inetAddress);

        if (networkAddress == static_cast<in_addr_t>(-1)) {
            return NetworkError("Can't get network address, error \"%s\"", *(hostEntry->h_addr_list));
        }

        struct sockaddr_in socketAddress = {
            .sin_family = AF_INET, .sin_port = ::htons(port), .sin_addr = { .s_addr = networkAddress },
        };

        if (::connect(fd, reinterpret_cast<struct sockaddr*>(&socketAddress), sizeof(struct sockaddr_in)) == -1) {
            return NetworkError("Can't connect socket, error %s", ::strerror(errno));
        }

        return {};
    }

    void HttpClient::disconnect() {
        if (isOpened()) {
            ::close(fd);
            fd = -1;
        }
    }

    bool HttpClient::isOpened() const { return fd != -1; }

    Result<std::int64_t, NetworkError> HttpClient::send() {
        std::int64_t totalBytes = 0;
        const std::size_t requestLength = request.length() + 1;

        while (totalBytes < requestLength) {
            std::int64_t wroteBytes = ::write(fd, request.data() + totalBytes, requestLength - totalBytes);

            if (wroteBytes == -1) {
                return NetworkError("Can't write to socket, error %s", ::strerror(errno));
            }

            totalBytes += wroteBytes;
        }

        return totalBytes;
    }

    Result<std::vector<std::string>, NetworkError> HttpClient::receive() const {
        char buffer[BUFFER_SIZE] = {0};
        std::int64_t readBytes = 0;
        std::vector<std::string> data;

        while ((readBytes = ::read(fd, buffer, BUFFER_SIZE)) > 0) {
            data.emplace_back(buffer, BUFFER_SIZE);
        }

        if (readBytes == -1) {
            return NetworkError("Can't read from socket, error %s", ::strerror(errno));
        }

        return data;
    }
}
