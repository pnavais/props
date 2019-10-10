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
std::unique_ptr<PropsSearchResult> PropsReader::find_value(const string &key, const std::list<PropsFile> &files)
{
    std::unique_ptr<PropsSearchResult> result(new PropsSearchResult(key));

	// Prepare regex
	std::stringstream regex_str;
	regex_str << "^" << key << "=(.+)";
    pcrecpp::RE regex(regex_str.str());
	string value_r;

	// TODO: Process using a queue and threads
	for (auto &file : files)
    {

        const string &fullPath = FileUtils::getAbsolutePath(file.getFileName());
	    std::ifstream infile(fullPath);

        if (infile) {
            // TODO: Process dividing file in chunks and in parallel
            string line;
            while (std::getline(infile, line)) {
                // Try to find the regex in line, and report results.
                if (!COMMENTED_LINE().PartialMatch(line)) {
                    if (regex.PartialMatch(line, &value_r)) {
                        infile.close();
                        result->add(file.getFileName(), value_r);
                        break;
                    }
                } else {
                    std::cout << rang::fgB::yellow << "Skipping commented line " << line << rang::fg::reset << std::endl;
                }
            }
        } else {
            std::cerr << rang::fgB::red << "File \"" << file.getFileName() << "\" not found" << rang::fg::reset << std::endl;
        }
    }

	return result;
}
