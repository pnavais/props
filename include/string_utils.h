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

#ifndef PROPS_STRING_UTILS_H
#define PROPS_STRING_UTILS_H

#include <iostream>
#include <list>

#define DEFAULT_PATTERN   " "
#define DEFAULT_SEPARATOR ","

const size_t MAX_STR_WIDTH = 10;

class StringUtils {

public:

    /**
     * Pads the string using an specific pattern up to a maximum length
     *
     * @param input the input string
     * @param pattern the pattern to use as filler
     * @param maxLength the maximum length
     * @return the padded string
     */
    static std::string padding(const std::string& input, const std::string& pattern = DEFAULT_PATTERN, const std::size_t& maxLength = MAX_STR_WIDTH);

    /**
     * Pads the string using an specific pattern up to a maximum length
     *
     * @param input the input string
     * @param maxLength the maximum length
     * @return the padded string
     */
    static std::string padding(const std::string& input, const std::size_t& maxLength);

    /**
     * Transforms a list of string values to a flat string
     * using the provided separator.
     *
     * @param args the list of arguments
     * @return the flat string
     */
    static std::string toFlatString(const std::list<std::string>& args, const std::string& separator = DEFAULT_SEPARATOR);
};

#endif //PROPS_STRING_UTILS_H
