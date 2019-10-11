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


#include <props_json_formatter.h>
#include <string_utils.h>

#define SPACER "  "

/**
 * Formats the given result in JSON
 * format appending to the given output stream.
 *
 * @param result the result
 * @param out the output stream
 */
void JsonPropsFormatter::format(const PropsSearchResult* result, std::ostream& out) const {

    if (result != nullptr) {
        const std::string &key = result->getKey();
        const auto &fileKeys = result->getFileKeys();

        if (!fileKeys.empty()) {

            // Build matches
            long numMatches = 0;
            std::ostringstream matches;
            matches << StringUtils::expand(SPACER, 4) << R"("files": [{)";
            std::string prefix;
            // Show files
            for (auto &fileKey : fileKeys) {
                numMatches += fileKey.second.size();
                matches << StringUtils::expand(SPACER, 6) << prefix << std::endl;
                matches << StringUtils::expand(SPACER, 8) << R"("name": ")" << fileKey.first << "\"," << std::endl;
                matches << StringUtils::expand(SPACER, 8) << R"("num_matches": )" << fileKey.second.size() << "," << std::endl;
                matches << StringUtils::expand(SPACER, 8) << R"("matches": [{)";

                // Show matches
                prefix = "";
                for (auto &match : fileKey.second) {
                    matches << StringUtils::expand(SPACER, 10) << prefix << std::endl;
                    matches << StringUtils::expand(SPACER, 12) << R"("full_match": ")" << match.fullLine_ << "\"," << std::endl;
                    matches << StringUtils::expand(SPACER, 12) << R"("value": ")" << match.value_ << "\"" << std::endl;
                    prefix = "},\n" + StringUtils::expand(SPACER, 10) + "{";
                }
                matches << StringUtils::expand(SPACER, 10) << "}" << std::endl;
                matches << StringUtils::expand(SPACER, 8) << "]" << std::endl;
                prefix = "},\n" + StringUtils::expand(SPACER, 6) + "{";
            }

            matches << StringUtils::expand(SPACER, 6) << "}" << std::endl;
            matches << StringUtils::expand(SPACER, 4) << "]" << std::endl;

            // Show header + matches
            out << "{" << std::endl;
            out << StringUtils::expand(SPACER,2) << R"("results": {)" << std::endl;
            out << StringUtils::expand(SPACER, 4) << R"("key": ")"<< key << "\"," << std::endl;
            out << StringUtils::expand(SPACER, 4) << R"("total_matches": )" << numMatches << "," << std::endl;
            out << StringUtils::expand(SPACER, 4) << R"("num_files": )" << fileKeys.size() << "," << std::endl;
            out << matches.str();
            out << StringUtils::expand(SPACER,2)  << "}\n}" << std::endl;
        }
    }
}
