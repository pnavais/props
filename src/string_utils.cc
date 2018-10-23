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

#include <string_utils.h>
#include <sstream>
#include <iomanip>

/**
 * Pads the string using an specific pattern up to a maximum length
 *
 * @param input the input string
 * @param pattern the pattern to use as filler
 * @param maxLength the maximum length
 * @return the padded string
 */
std::string StringUtils::padding(const std::string& input, const std::string& pattern, const std::size_t& maxLength)
{
    std::string output;

    if (maxLength > input.length()) {
        std::ostringstream out;
        out << input;

        while (static_cast<std::string::size_type>(out.tellp()) < maxLength) {
            out << pattern;
        }

        output = out.str().erase(maxLength, std::string::npos);
    } else {
        output = input;
    }

    return output;
}


/**
 * Pads the string using an specific pattern up to a maximum length
 *
 * @param input the input string
 * @param maxLength the maximum length
 * @return the padded string
 */
std::string StringUtils::padding(const std::string& input, const std::size_t& maxLength)
{
    return StringUtils::padding(input, DEFAULT_PATTERN, maxLength);
}

/**
 * Transforms a list of string values to a flat string
 * using the provided separator.
 *
 * @param args the list of arguments
 * @return
 */
std::string StringUtils::toFlatString(const std::list<std::string>& args, const std::string& separator)
{
    std::ostringstream out;

    std::string prefix;
    for (auto& arg : args) {
        out << prefix << arg;
        prefix = separator;
    }

    return out.str();
}

/**
 * Converts a string to upper case.
 *
 * @param input the input string
 */
std::basic_string<char> StringUtils::toUpper(const std::basic_string<char>& input) {
    std::basic_string<char> output = input;
    for (auto &p : output) {
        p = static_cast<char>(toupper(p));
    }

    return output;
}

/**
 * Breaks the input text into lines of the same size avoiding splitting
 * words at the end of the string.
 *
 * @param input the input string
 * @param width the limit
 *
 * @return the list of wrapped strings
 */
std::list<std::string> StringUtils::wraptext(const std::string& input, const std::size_t width) {
    size_t curpos  = 0;
    size_t nextpos = 0;

    std::list<std::string> lines;
    std::string substr = input.substr(curpos, width + 1);

    while (substr.length() == width + 1 && (nextpos = substr.rfind(' ')) != std::string::npos) {
        lines.push_back(input.substr(curpos, nextpos));
        curpos += nextpos + 1;
        substr = input.substr(curpos, width + 1);
    }

    if (curpos != input.length())
        lines.push_back(input.substr(curpos, std::string::npos));

    return lines;
}
