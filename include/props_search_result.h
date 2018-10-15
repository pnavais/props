#include <utility>

/*
 * Copyright 2018 Pablo Navais
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

#ifndef PROPS_PROPS_SEARCH_RESULT_H
#define PROPS_PROPS_SEARCH_RESULT_H

#include "props_result.h"
#include <string>
#include <map>
#include <list>

namespace p_search_res {
    typedef std::map<std::string, std::list<std::string>> result_map;
}

class PropsSearchResult : public PropsResult {

public:

    /**
     * Creates a results for the given key
     *
     * @param key the key
     */
    explicit PropsSearchResult(const std::string& key) : key_(key) {}

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
    void add(const std::string &file, const std::string &value);

    /**
     * Retrieves the results for the given key in the master file
     * and optionally (if global mode enabled) in the rest of tracked files.
     *
     * @param results the results for the master and optionally other files
     */
    p_search_res::result_map get() const;

    /**
     * Retrieves the results for the given file.
     *
     * @param fileName the list of file names to retrieve results from
     * @param results the results for the given file
     */
    p_search_res::result_map get(const std::list<std::string> &fileNames) const;

    /**
     * Formats the contents of the result in an
     * output stream.
     *
     * @param out the stream with formatted result.
     */
    void format(std::ostream& out) const;

private:

    p_search_res::result_map fileKeys_;
    std::string key_;
};


#endif //PROPS_PROPS_SEARCH_RESULT_H