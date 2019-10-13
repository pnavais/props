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

#include <result.h>
#include <parser/toml.hpp>
#include <init_exception.h>
#include "props_config.h"

/**
 * Private function prototypes.
 */
void convertValue(const toml::value& value, std::string& s);
bool readTableProperties(const toml::table& tab, const std::string& sectionName, std::map<std::string, std::string>& properties);

/**
 * Converts a TOML value to a string.
 *
 * @param value the TOML value to convert
 * @param s the output string
 */
void convertValue(const toml::value& value, std::string& s) {
    if (value.is_string()) {
        s = value.as_string();
    } else {
        std::ostringstream valStream;
        valStream << value;
        s = valStream.str();
    }
}

/**
 * Retrieves the actual value for the given key or the
 * given default value if not found.
 *
 * @param key the key to lookup
 * @param defaultValue the default value
 * @return  the value or default value if not found
 */

const std::string* PropsConfig::getValue(const std::string& key, const std::string& defaultValue) const {
    const auto& val = getValue(key);
    return (val == nullptr) ? &defaultValue : val;
}

/**
 * Retrieves the actual value for the given key
 * or null if not founds.
 *
 * @param key the key to lookup
 * @return the value or null if not found
 */
const std::string* PropsConfig::getValue(const std::string& key) const {
    const std::string* val = nullptr;
    if (properties_.count(key)) {
        val = &(properties_.at(key));
    }

    return val;
}

/**
 * Parses the configuration file and throws
 * an InitializationException if the configuration
 * file is not valid.
 */
void PropsConfig::parseConfig() {
    auto& configFilePath = config::CONFIG_FILE_PATH();
    Result result{res::VALID};

    if (FileUtils::fileExists(configFilePath)) {
        try {
            const auto data = toml::parse(configFilePath);
            const auto& tab  = toml::get<toml::table>(data);

            readTableProperties(tab, "General", properties_);
            readTableProperties(tab, "Search",  properties_);
            readTableProperties(tab, "Alias",   properties_);
        } catch (std::exception &e) {
            throw InitializationException("Error parsing configuration file. Details : " + std::string(e.what()));
        }
    }
}

/**
 * Read a table's properties from the parsed data.
 *
 * @param data the parsed data
 * @param sectionName the section name
 * @param properties the properties map
 */
bool readTableProperties(const toml::table& tab, const std::string& sectionName, std::map<std::string, std::string>& properties) {

    bool res = true;
    if (tab.count(sectionName) != 0) {
        const auto& section = toml::get<toml::table>(tab.at(sectionName));

        for (auto& generalEntry : section) {
            const toml::value& value = generalEntry.second;
            std::string s;
            convertValue(value, s);

            properties[StringUtils::toLower(sectionName)+"."+generalEntry.first] = s;
        }
    } else {
        res = false;
    }

    return res;
}

