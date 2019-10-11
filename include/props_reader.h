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

namespace search {

    static const char KEY_SEPARATOR[] = "search.key_separator";
    static const char DEFAULT_KEY_SEPARATOR[] = "=";
    static const char KEY_IGNORE_CASE[] = "search.ignore_case";
    static const bool DEFAULT_IGNORE_CASE = false;
    static const char KEY_ALLOW_PARTIAL_MATCH[] = "search.allow_partial_match";
    static const bool DEFAULT_ALLOW_PARTIAL_MATCH = false;
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
     *  @return the results of the search
     */
     static std::unique_ptr<PropsSearchResult> find_value(const p_search_res::SearchOptions& searchOptions, const std::list<PropsFile>& files);

};

#endif
