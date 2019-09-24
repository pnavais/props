/*
 * Copyright 2019 Pablo Navais
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "file_utils.h"
#include <cstdlib>
#include <climits>
#include <sys/stat.h>
#include "config_static.h"

#if defined(IS_LINUX) || defined(IS_MAC)
#include <unistd.h>
#include <pwd.h>
#endif


/**
 * Checks if a given file exists and is accessible.
 *
 * @param fileName the path to the file to check
 * @return true if file exists, false otherwise
 */
bool FileUtils::fileExists(const std::string& fileName) noexcept {
    struct stat buffer{};
    return (stat (fileName.c_str(), &buffer) == 0);
}

/**
 * Retrieves the absolute path of a given file.
 *
 * @param filePath the path to the file
 * @return retrieves the absolute file path
 */
std::string	FileUtils::getAbsolutePath(const std::string& filePath) noexcept {

    std::string fullFilePath = filePath;
    char buff[PATH_MAX];

    if (realpath(filePath.c_str(), buff) != nullptr) {
        fullFilePath = std::string(buff);
    }

    return fullFilePath;
}

/**
 * Retrieves the temporary directory
 *
 * @return the temporary directory
 */
std::string FileUtils::getTmpDir() {
    const char* tmpDir = "";

#if defined(IS_LINUX) || defined(IS_MAC)
    if ((tmpDir = getenv("TMPDIR")) == nullptr) {
        tmpDir = "/tmp";
    }
#elif defined(IS_WIN)
    if ((tmpDir = getenv("TEMP")) == nullptr) {
        if ((tmpDir = getenv("TMP")) == nullptr) {
            tmpDir = "";
        }
    }
#endif

    return std::string(tmpDir) + ftl::pathSeparator;
}

/**
 * Retrieves the temporary directory
 *
 * @return the temporary directory
 */
std::string FileUtils::getHomeDir() {
    const char* homeDir = "";

#if defined(IS_LINUX) || defined(IS_MAC)
    if ((homeDir = getenv("HOME")) == nullptr) {
        homeDir = getpwuid(getuid())->pw_dir;
    }
#elif defined(IS_WIN)
    const char* homePath;
    homePath = getenv("HOMEPATH");

    const char* homeDrive;
    homeDrive = getenv("HOMEDRIVE");

    if ((homePath != nullptr) && (homeDrive != nullptr)) {
        homeDir = (std::string(homePath) + std::string(homeDrive)).c_str();
    } else {
        if ((homeDir = getenv("USER")) == nullptr) {
            homeDir = "";
        };
    }
#endif

    return std::string(homeDir) + ftl::pathSeparator;
}

