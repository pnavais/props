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

#include <props_search_result.h>
#include <props_file_tracker.h>

#include "props_search_result.h"

/**
 * Retrieves the key.
 *
 * @return the key
 */
const std::string &PropsSearchResult::getKey() const {
    return key_;
}

/**
 * Appends the pair key/value found in the given file
 * to the results map.
 *
 * @param file the file where the key was found
 * @param key the searched key
 * @param value the value found
 */
void PropsSearchResult::add(const std::string &file, const std::string &value) {
    this->fileKeys_[file].push_back(value);
}

/**
 * Retrieves the results for the given key in the master file
 * or optionally (if global mode enabled) in the rest of tracked
 * files.
 *
 * @param key the searched key
 * @param results the results for the given key
 */
p_search_res::result_map PropsSearchResult::get() const {
    std::string masterFile = PropsFileTracker::getMaster();
    return get(std::list<std::string>({masterFile}));
}

/**
 * Retrieves the results for the given file.
 *
 * @param fileName the file names to retrieve results
 * @param key the searched key
 * @param list the results for the given key
 */
p_search_res::result_map PropsSearchResult::get(const std::list<std::string> &fileNames) const {
    p_search_res::result_map results;
    for (auto &fileName : fileNames) {
        if (this->fileKeys_.find(fileName) != this->fileKeys_.end()) {
            results[fileName] = this->fileKeys_.at(fileName);
        }
    }

    return results;
}

/**
 * Formats the contents of the result in an
 * output stream.
 *
 * @param out the stream with formatted result.
 */
void PropsSearchResult::format(std::ostream& out) const {

}
