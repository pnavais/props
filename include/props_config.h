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

#ifndef PROPS_CONFIG_H
#define PROPS_CONFIG_H

#include <file_utils.h>
#include <map>
#include "string_utils.h"

/**
 * Namespace for constants
 */
namespace config {

    static const char* CONFIG_FILE_NAME = "props.conf";

    // The path to the config folder
    inline const std::string& CONFIG_FOLDER_PATH() {
        static const std::string CONFIG_FOLDER_PATH =
                FileUtils::getHomeDir() + ftl::pathSeparator + ".config" + ftl::pathSeparator;
        return CONFIG_FOLDER_PATH;
    }

    // The props folder name
    inline const std::string& PROPS_FOLDER() {
        static const std::string PROPS_FOLDER = std::string("props") + ftl::pathSeparator;
        return PROPS_FOLDER;
    }

    inline const std::string& CONFIG_FULL_PATH() {
        static const std::string CONFIG_FULL_PATH = CONFIG_FOLDER_PATH() + PROPS_FOLDER();
        return CONFIG_FULL_PATH;
    }
    inline const std::string& CONFIG_FILE_PATH() {
        static const std::string CONFIG_FILE_PATH = CONFIG_FULL_PATH() + CONFIG_FILE_NAME;
        return CONFIG_FILE_PATH;
    }
}

class PropsConfig {
public:

    /**
    * Retrieves the singleton instance
    * @return the singleton instance
    */
    static PropsConfig& getDefault() {
        static PropsConfig instance;
        return instance;
    }

    /**
     * Reads the configuration file initializing
     * the properties.
     */
    void init() {
        parseConfig();
    }

    /**
     * Retrieves the value as the target type (if possible)
     * returning a flag specifying if the conversion succeeded
     * or value was not found.
     *
     * @tparam T the target type
     * @param key the key
     * @return the value or null if not found
     */
    template <class T>
    T getValue(const std::string& key, bool& res) {
        const std::string* val = getValue(key);
        T target;
        if (val != nullptr) {
            res = StringUtils::from_string<T>(target,*val);
        } else {
            res = false;
        }
        return target;
    }

    /**
     * Retrieves the value as the target type (if possible)
     * returning a flag specifying if the conversion succeeded
     * or value was not found.
     *
     * @tparam T the target type
     * @param key the key
     * @return the value or null if not found
     */
    template <class T>
    T getValue(const std::string& key, const T& defaultValue) {
        const std::string* val = getValue(key);

        bool res = true;
        T target = defaultValue;
        if (val != nullptr) {
            res = StringUtils::from_string<T>(target,*val);
        }

        target = (!res) ? defaultValue : target;
        return target;
    }

    /**
     * Retrieves the actual value for the given key or the
     * given default value if not found.
     *
     * @param key the key to lookup
     * @param defaultValue the default value
     * @return  the value or default value if not found
     */
    const std::string* getValue(const std::string& key, const std::string& defaultValue) const;

    /**
     * Retrieves the actual value for the given key
     * or null if not founds.
     *
     * @param key the key to lookup
     * @return the value or null if not found
     */
    const std::string* getValue(const std::string& key) const;

private:

    /**
     * Default constructor
     */
    PropsConfig() = default;

    /**
     * Parses the configuration file and throws
     * an InitializationException if the configuration
     * file is not valid.
     */
    void parseConfig();

    std::map<std::string, std::string> properties_;

};


#endif //PROPS_CONFIG_H
