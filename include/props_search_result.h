#include <utility>

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

#ifndef PROPS_SEARCH_RESULT_H
#define PROPS_SEARCH_RESULT_H

#include "props_result.h"
#include <string>
#include <map>
#include <list>

namespace p_search_res {
    typedef struct Match {
            std::string fullLine_;
            std::string value_;
            bool caseSensitive_;
    } Match;

    typedef std::map<std::string, std::list<Match>> result_map;
}

class PropsSearchResult : public PropsResult {

public:

    /**
     * Creates a results for the given key
     *
     * @param key the key
     */
    explicit PropsSearchResult(std::string key) : key_(std::move(key)) {}

    /**
     * Retrieves the key
     *
     * @return the key
     */
    const std::string &getKey() const;

    /**
     * Appends the pair key/value found in the given file
     * to the results map.
     *
     * @param file the file where the key was found
     * @param value the value found
     */
    void add(const std::string &file, const p_search_res::Match &value);

    /**
     * Retrieves the results for the given file.
     *
     * @param fileName the list of file names to retrieve results from
     * @param results the results for the given file
     */
    p_search_res::result_map get(const std::list<std::string> &fileNames) const;

    /**
     * Retrieves all results
     *
     * @param results the results for all files
     */
    const p_search_res::result_map& getFileKeys() const {
        return fileKeys_;
    }

    /**
     * Formats the contents of the result in an
     * output stream.
     *
     * @param out the stream with formatted result.
     */
    void format(std::ostream& out) const override;

    /**
     * Enables/disables JSON output
     *
     * @param enableJson true to enable, false otherwise
     */
    void setEnableJson(const bool& enableJson) {
        enableJson_ = enableJson;
    }

    /**
     * Check if JSON output is enabled
     *
     * @return true if JSON output enabled, false otherwise
     */
    const bool& isEnableJson() const {
        return enableJson_;
    }

private:

    p_search_res::result_map fileKeys_;
    std::string key_;
    bool enableJson_{false};
};


#endif //PROPS_SEARCH_RESULT_H
