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

#include "props_reader.h"
#include "props_search_result.h"
#include "rang.hpp"
#include <fstream>
#include <sstream>
#include <pcrecpp.h>
#include <file_utils.h>
#include <props_config.h>

// Prototypes for globals
const pcrecpp::RE &COMMENTED_LINE();

/**
 * Retrieves the regular expression for
 * commented lines.
 *
 * @return the regex for commented lines
 */
const pcrecpp::RE &COMMENTED_LINE() {
    static const pcrecpp::RE COMMENTED_LINE("^#");
    return COMMENTED_LINE;
}

/**
 * Finds the value for the key in the specified file.
 *
 * @param key the key to search
 * @param file the source file
 * @return the value for the key in the file
 */
std::unique_ptr<PropsSearchResult> PropsReader::find_value(const p_search_res::SearchOptions &searchOptions, const std::list<PropsFile> &files)
{
    pcrecpp::RE_Options opt;
    const std::string& key    = searchOptions.key_;
    std::string keySeparator  = searchOptions.separator_;
    p_search_res::Opt caseSensitive = searchOptions.caseSensitive_;
    p_search_res::Opt partialMatch  = searchOptions.partialMatch_;
    const bool& matchValue    = searchOptions.matchValue_;

    // Get Default values
    if (keySeparator.empty()) {
        keySeparator = PropsConfig::getDefault().getValue<std::string>(search::KEY_SEPARATOR, search::DEFAULT_KEY_SEPARATOR);
    }

    if (caseSensitive == p_search_res::DEFAULT) {
        bool ignore_case = PropsConfig::getDefault().getValue<bool>(search::KEY_IGNORE_CASE, search::DEFAULT_IGNORE_CASE);
        caseSensitive = (ignore_case) ? p_search_res::NO_OPT : p_search_res::USE_OPT;
    }

    if (partialMatch == p_search_res::DEFAULT) {
        bool allow_partial_match = PropsConfig::getDefault().getValue<bool>(search::KEY_ALLOW_PARTIAL_MATCH, search::DEFAULT_ALLOW_PARTIAL_MATCH);
        partialMatch = (allow_partial_match) ? p_search_res::USE_OPT : p_search_res::NO_OPT;
    }

    std::unique_ptr<PropsSearchResult> result(new PropsSearchResult(searchOptions));

	// Prepare regex
	std::stringstream regex_str;
    opt.set_caseless(caseSensitive == p_search_res::NO_OPT);
    std::string partStr = (partialMatch == p_search_res::USE_OPT) ? ".*" : "";

    if (matchValue) {
        regex_str << "^(.+)" << keySeparator << partStr << key << partStr << "$";
    } else {
        regex_str << "^" << partStr << key << partStr << keySeparator << "(.+)";
    }
    pcrecpp::RE regex(regex_str.str(), opt);
	std::string value_r;

	// TODO: Process using a queue and threads
	for (auto &file : files)
    {
        const std::string &fullPath = FileUtils::getAbsolutePath(file.getFileName());
	    std::ifstream infile(fullPath);

        if (infile) {
            // TODO: Process dividing file in chunks and in parallel (mmap ?)
            std::string line;
            while (std::getline(infile, line)) {
                // Try to find the regex in line, and report results.
                if (!COMMENTED_LINE().PartialMatch(line)) {
                    if (regex.PartialMatch(line, &value_r)) {
                        result->add(file.getFileName(), p_search_res::Match{ line, value_r, (caseSensitive == p_search_res::USE_OPT)});
                    }
                }
            }

            infile.close();
        } else {
            std::cerr << rang::fgB::red << "File \"" << file.getFileName() << "\" not found" << rang::fg::reset << std::endl;
        }
    }

	return result;
}
