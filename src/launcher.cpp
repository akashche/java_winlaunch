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

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#define UNICODE
#define _UNICODE
#include <windows.h>

#include "utils.hpp"

//#define JL_JAVA_RELATIVE_PATH "jre/bin/java"
//#define JL_JAR_RELATIVE_PATH "path/to/launcher.jar"
//#define JL_JAR_ARGUMENTS "-i foo -o bar"

namespace winlaunch {

void launch() {
    auto localdir = get_process_dir();
    auto cmd = "\"" + localdir + JL_JAVA_RELATIVE_PATH + "\" -jar \"" + localdir + JL_JAR_RELATIVE_PATH + "\" " + JL_JAR_ARGUMENTS;
    std::cout << "Is due to launch cmd: [" + cmd + "]" << std::endl;
    auto vec = widen(cmd);
    STARTUPINFOW si;
    memset(&si, 0, sizeof(STARTUPINFOW));
    si.cb = sizeof(STARTUPINFOW);
    si.dwFlags = STARTF_FORCEONFEEDBACK;
    const unsigned long flags = CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS;
    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(PROCESS_INFORMATION));
    // run process
    int success = CreateProcessW(NULL, vec.data(), NULL, NULL, TRUE, flags, NULL, NULL, &si, &pi);
    if (0 == success) {
        std::cout << errcode_to_string(GetLastError()) << std::endl;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

}

int main() {
    winlaunch::launch();

    return 0;
}
