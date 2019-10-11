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


#include <props_simple_formatter.h>
#include <string_utils.h>

/**
 * Formats the given result appending
 * to the given output stream.
 *
 * @param result the result
 * @param out the output stream
 */
void SimplePropsFormatter::format(const PropsSearchResult* result, std::ostream& out) const {

    if (result != nullptr) {
        const std::string &key = result->getSearchOptions().key_;
        const auto &fileKeys = result->getFileKeys();

        if (!fileKeys.empty()) {
            for (auto &fileKey : fileKeys) {
                out << std::endl << rang::style::bold << rang::fgB::green << fileKey.first << rang::style::reset
                    << std::endl;
                int i = 1;
                for (auto &match : fileKey.second) {
                    out << rang::style::bold << rang::fgB::yellow << i << rang::style::reset << ":"
                        << StringUtils::highlight(match.fullLine_, key, match.caseSensitive_) << std::endl;
                    i++;
                }
            }
        }
    }
}
