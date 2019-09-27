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
#include "config_static.h"

#if defined(IS_LINUX) || defined(IS_MAC)
#include <unistd.h>
#include <pwd.h>
#endif

#if defined(__cplusplus) && __cplusplus >= 201703L && defined(__has_include)
#if __has_include(<filesystem>)
#define GHC_USE_STD_FS
#include <filesystem>
namespace fs = std::filesystem;
#endif
#endif
#ifndef GHC_USE_STD_FS
#include <ghc/filesystem.hpp>
#include <iostream>

namespace fs = ghc::filesystem;
#endif

/**
 * Create directory and sub-folders recursively.
 *
 * @param directory the directory to create
 * @return true if the operation succeeded, false otherwise
 */
bool FileUtils::createDirectories(const std::string& directory) noexcept {
    bool result = true;
    try {
        fs::path p{directory};
        p.remove_filename();
        fs::create_directories(p);
    } catch (fs::filesystem_error& e) {
        result = false;
    }

    return result;
}

/**
 * Checks if a given file exists and is accessible.
 *
 * @param fileName the path to the file to check
 * @return true if file exists, false otherwise
 */
bool FileUtils::fileExists(const std::string& fileName) noexcept {
    return fs::exists(fileName);
}

/**
 * Retrieves the absolute path of a given file.
 *
 * @param filePath the path to the file
 * @return retrieves the absolute file path
 */
std::string	FileUtils::getAbsolutePath(const std::string& filePath) noexcept {
    return fs::absolute(filePath);
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

    return std::string(homeDir);
}

/**
 * Renames a given file from source to target.
 *
 * @param sourceFile the source file
 * @param targetFile the target file
 * @return true if the operation succeded, false otherwise
 */
bool FileUtils::rename(const std::string& sourceFile, const std::string& targetFile) {
    bool res = true;
    try {
        fs::rename(sourceFile, targetFile);
    } catch (fs::filesystem_error& e) {
        res = false;
    }

    return res;
}

/**
* Renames a given file from source to target.
*
* @param file the file to remove
* @return true if the operation succeeded, false otherwise
*/
bool FileUtils::remove(const std::string& file) {
    return fileExists(file) && fs::remove(file);
}

