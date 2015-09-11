/*
 * Copyright 2015, akashche at redhat.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>
#include <algorithm>
#define UNICODE
#define _UNICODE
#include <windows.h>

#include "utils.hpp"

namespace {

class LocalFreeDeleter {
public:
    void operator()(wchar_t* buf) {
       LocalFree(buf);
    }
};

} // namespace

namespace WinLaunchException {

std::string to_hex_string(int i) {
    std::stringstream stream{};
    stream << "0x" << std::hex << i;
    return stream.str();
}

std::vector<wchar_t> widen(const std::string& st) {
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, st.c_str(), static_cast<int>(st.length()), nullptr, 0);
    if (0 == size_needed) throw WinLaunchException(std::string("Error on string widen calculation,") +
            " string: [" + st + "], error: [" + errcode_to_string(GetLastError()) + "]");
    std::vector<wchar_t> vec{};
    vec.resize(size_needed + 1);
    int chars_copied = MultiByteToWideChar(CP_UTF8, 0, st.c_str(), static_cast<int>(st.size()), vec.data(), size_needed);
    if (chars_copied != size_needed) throw WinLaunchException(std::string("Error on string widen execution,") +
            " string: [" + st + "], error: [" + errcode_to_string(GetLastError()) + "]");
    return vec;
}

std::string narrow(const wchar_t* wstring, size_t length) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstring, static_cast<int>(length), nullptr, 0, nullptr, nullptr);
    if (0 == size_needed) throw WinLaunchException(std::string("Error on string narrow calculation,") +
            " string length: [" + std::to_string(length) + "], error code: [" + std::to_string(GetLastError()) + "]");
    std::vector<char> vec{};
    vec.resize(size_needed);
    int bytes_copied = WideCharToMultiByte(CP_UTF8, 0, wstring, static_cast<int>(length), vec.data(), size_needed, nullptr, nullptr);
    if (bytes_copied != size_needed) throw WinLaunchException(std::string("Error on string narrow execution,") +
            " string length: [" + std::to_string(vec.size()) + "], error code: [" + std::to_string(GetLastError()) + "]");
    std::string st{vec.begin(), vec.end()};
    return st;
}

std::string errcode_to_string(unsigned long code) noexcept {
    if (0 == code) return std::string{};
    wchar_t* buf_p = nullptr;
    size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
            reinterpret_cast<wchar_t*>(&buf_p), 0, nullptr);
    if (0 == size) {
        return "Cannot format code: [" + std::to_string(code) + "]" +
            " into message, error code: [" + std::to_string(GetLastError()) + "]";
    }
    auto buf = std::unique_ptr<wchar_t, LocalFreeDeleter>(buf_p, LocalFreeDeleter{});
    if (size <= 2) {
        return "code: [" + std::to_string(code) + "], message: []";
    }
    try {
        std::string msg = narrow(buf.get(), size - 2);
        return "code: [" + std::to_string(code) + "], message: [" + msg + "]";
    } catch(const std::exception& e) {
        return "Cannot format code: [" + std::to_string(code) + "]" +
            " into message, narrow error: [" + e.what() + "]";
    }
}

std::string get_process_dir() {
    std::vector<wchar_t> vec{};
    vec.resize(MAX_PATH);
    unsigned long success = GetModuleFileName(nullptr, vec.data(), vec.size());
    if (0 == success) throw WinLaunchException(std::string("Error getting current executable dir,") +
            " error: [" + errcode_to_string(GetLastError()) + "]");
    auto path = narrow(vec.data(), vec.size());
    std::replace(path.begin(), path.end(), '\\', '/');
    auto sid = path.rfind('/');
    return std::string::npos != sid ? path.substr(0, sid + 1) : path;
}

WinLaunchException::WinLaunchException(const std::string& message) :
message(message) { }

const char* WinLaunchException::what() const noexcept {
    return message.c_str();
}

}
