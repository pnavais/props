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
#include <exec_exception.h>

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
std::unique_ptr<PropsSearchResult> PropsReader::find_value(p_search_res::SearchOptions &searchOptions, const std::list<PropsFile> &files)
{
    std::unique_ptr<PropsSearchResult> result(new PropsSearchResult(searchOptions));
    const std::string& input = searchOptions.key_;

    // Amend options if defaults needed
    fixSearchOptions(searchOptions);

    // Regex options
    pcrecpp::RE_Options opt;
    opt.set_caseless((searchOptions.caseSensitive_ == p_search_res::NO_OPT));

    // Build regex
    std::string regex_in;
    buildRegex(searchOptions, regex_in);
    pcrecpp::RE regex(regex_in, opt);

    std::string value_k;
	std::string value_r;

    if (regex.NumberOfCapturingGroups() > 2) {
        throw ExecutionException("Too many capture groups specified");
    }

	// TODO: Process using a queue and threads
	for (auto &file : files)
    {
        const std::string &fullPath = FileUtils::getAbsolutePath(file.getFileName());
	    std::ifstream infile(fullPath);

        if (infile) {
            // TODO: Process dividing file in chunks and in parallel (mmap ?)
            std::string line;
            while (std::getline(infile, line)) {
                // Try to find the regex in line, and keep results.
                if (!COMMENTED_LINE().PartialMatch(line)) {
                    if (regex.PartialMatch(line, &value_k, &value_r)) {
                        result->add(file.getFileName(), p_search_res::Match{ input, searchOptions, line, value_k, value_r });
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

/**
 * Amend input options if default values needed.
 *
 * @param searchOptions the search options
 */
void PropsReader::fixSearchOptions(p_search_res::SearchOptions &searchOptions) {

    // Amend search options by retrieving default values if needed
    if (searchOptions.caseSensitive_ == p_search_res::DEFAULT) {
        bool ignore_case = PropsConfig::getDefault().getValue<bool>(search::KEY_IGNORE_CASE, search::DEFAULT_IGNORE_CASE);
        searchOptions.caseSensitive_ = (ignore_case) ? p_search_res::NO_OPT : p_search_res::USE_OPT;
    }

    if (searchOptions.partialMatch_ == p_search_res::DEFAULT) {
        bool allow_partial_match = PropsConfig::getDefault().getValue<bool>(search::KEY_ALLOW_PARTIAL_MATCH, search::DEFAULT_ALLOW_PARTIAL_MATCH);
        searchOptions.partialMatch_ = (allow_partial_match) ? p_search_res::USE_OPT : p_search_res::NO_OPT;
    }

    if (searchOptions.separator_.empty()) {
        searchOptions.separator_ = PropsConfig::getDefault().getValue<std::string>(search::KEY_SEPARATOR, search::DEFAULT_KEY_SEPARATOR);
    }
}

/**
 * Builds the search regular expression from the given search options.
 *
 * @param searchOptions the search options
 * @param regex_str the built string regex
 */
void PropsReader::buildRegex(const p_search_res::SearchOptions& searchOptions, std::string& regex_str) {
    std::string key   = searchOptions.key_;
    const bool& matchValue   = searchOptions.matchValue_;

    if (!searchOptions.isRegex_) {
        key = pcrecpp::RE::QuoteMeta(key);
    }

    // Prepare regex
    std::stringstream regex_stream;
    std::string partStr = (searchOptions.partialMatch_ == p_search_res::USE_OPT) ? ".*" : "";

    if (matchValue) {
        regex_stream << "^(.+)" << searchOptions.separator_  << partStr << "(" << key  << ")" << partStr << "$";
    } else {
        regex_stream << "^" << partStr << "(" << key << ")" << partStr << searchOptions.separator_  << "(.+)";
    }

    regex_str = regex_stream.str();

}
