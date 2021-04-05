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

#ifndef PROPS_READER_H
#define PROPS_READER_H

#include <iostream>
#include <list>
#include <memory>
#include <props_search_result.h>
#include <props_file.h>
#include <deque>

/**
 * Namespace for search options
 */
namespace search {
    static const char KEY_SEPARATOR[] = "search.key_separator";
    static const char DEFAULT_KEY_SEPARATOR[] = "=";
    static const char KEY_IGNORE_CASE[] = "search.ignore_case";
    static const bool DEFAULT_IGNORE_CASE = false;
    static const char KEY_ALLOW_PARTIAL_MATCH[] = "search.allow_partial_match";
    static const bool DEFAULT_ALLOW_PARTIAL_MATCH = false;
    static const char KEY_ENABLE_HIGHLIGHT[] = "search.highlight_results";
    static const bool DEFAULT_ENABLE_HIGHLIGHT = true;

    typedef struct FileSearchData {
        PropsSearchOptions* searchOptions_;
        void* regex_;
        std::deque<PropsFile>* filesQueue_;
        PropsSearchResult* searchResult_;
    } FileSearchData;
}

/**
 * Performs the lookup of values in properties files
 */
class PropsReader {

public:

    /**
     *  Finds the value of the given key searching in the mater tracked file
     *  or globally on every tracked file.
     *
     *  @param key the key to find
     *  @param files the list of files to search
     *  @param global the flag enabling global search
     *
     * @return the results of the search
     */
     static std::unique_ptr<PropsSearchResult> processSearch(PropsSearchOptions& searchOptions, const std::list<PropsFile>& files);

private:


    /**
     * Amend input options if default values needed.
     *
     * @param searchOptions the search options
     */
    static void fixSearchOptions(PropsSearchOptions& searchOptions);

    /**
     * Builds the search regular expression from the given search options.
     *
     * @param searchOptions the search options
     * @param regex_str the built string regex
     */
    static void buildRegex(const PropsSearchOptions& searchOptions, std::string& regex_str);

    /**
     * Retrieves the search data for the given options and file list.
     *
     * @param searchOptions the search options
     * @param files the list of files to process
     * @return the built search data
     */
    static search::FileSearchData buildSearchData(PropsSearchOptions& searchOptions, const std::list<PropsFile>& files);

};

#endif
