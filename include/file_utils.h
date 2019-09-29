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

#ifndef PROPS_FILE_UTILS_H
#define PROPS_FILE_UTILS_H

#include <string>

namespace ftl {
    static const char pathSeparator =
#if defined(IS_WIN)
            '\\';
#else
            '/';
#endif
}

class FileUtils {

public:

    /**
     * Removes default constructor
     */
    FileUtils() = delete;

    /**
     * Checks if a given file exists and is accessible.
     *
     * @param fileName the path to the file to check
     * @return true if file exists, false otherwise
     */
    static bool fileExists(const std::string& fileName) noexcept;

    /**
    * Retrieves the absolute path of a given file.
    *
    * @param filePath the path to the file
    * @return retrieves the absolute file path
    */
    static std::string getAbsolutePath(const std::string& filePath) noexcept ;

    /**
     * Create directory and sub-folders recursively.
     *
     * @param directory the directory to create
     * @return true if the operation succeeded, false otherwise
     */
    static bool createDirectories(const std::string& directory) noexcept;

    /**
    * Retrieves the user's home directory,
    *
    * @return the user's home directory
    */
    static std::string getHomeDir();

    /**
     * Renames a given file from source to target.
     *
     * @param sourceFile the source file
     * @param targetFile the target file
     * @return true if the operation succeeded, false otherwise
     */
    static bool rename(const std::string& sourceFile, const std::string& targetFile);

    /**
    * Renames a given file from source to target.
    *
    * @param file the file to remove
    * @return true if the operation succeeded, false otherwise
    */
    static bool remove(const std::string& file);
};

#endif //PROPS_FILE_UTILS_H
