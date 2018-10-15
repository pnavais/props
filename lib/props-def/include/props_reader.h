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

#ifndef PROPS_READER_H
#define PROPS_READER_H

#include <iostream>
#include <list>
#include <props_search_result.h>

using namespace std;

class PropsReader {

public:

    /**
     * Finds the value of the given key searching in the mater tracked file
     * or globally on every tracked file.
     *
     * @param key the key to find
     * @param global the flag enabling global search
     * @return the results of the search
     */
     PropsSearchResult find_value (const string& key, const bool& global = false);

    /**
     *  Finds the value of the given key searching in the mater tracked file
     *  or globally on every tracked file.
     *
     *  @param key the key to find
     *  @param files the list of files to search
     *  @param global the flag enabling global search
     *  @return the results of the search
     */
     PropsSearchResult find_value (const string& key, const std::list<string>& files);

};

#endif
