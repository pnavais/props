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
#include <props_config.h>
#include <props_reader.h>

/**
 * Formats the given result appending
 * to the given output stream.
 *
 * @param result the result
 * @param out the output stream
 */
void SimplePropsFormatter::format(const PropsSearchResult* result, std::ostream& out) const {

    if (result != nullptr) {

        const auto &fileKeys = result->getFileKeys();

        bool enableHighlight = PropsConfig::getDefault().getValue<bool>(search::KEY_ENABLE_HIGHLIGHT, search::DEFAULT_ENABLE_HIGHLIGHT);

        if (!fileKeys.empty()) {
            for (auto &fileKey : fileKeys) {
                out << std::endl << rang::style::bold << rang::fgB::green << fileKey.first << rang::style::reset
                    << std::endl;
                int i = 1;
                for (auto &match : fileKey.second) {
                    const std::string& match_str = (enableHighlight)
                        ? StringUtils::highlight(match.fullLine_, ((match.searchOptions_.matchValue_) ? match.value_ : match.key_), match.searchOptions_.caseSensitive_)
                        : match.fullLine_;

                    out << rang::style::bold << rang::fgB::yellow << i << rang::style::reset << ":"
                        << match_str << std::endl;
                    i++;
                }
            }
        }
    }
}
