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

#ifndef PROPS_STRING_UTILS_H
#define PROPS_STRING_UTILS_H

#include <iostream>
#include <list>
#include <sstream>

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

    /**
     * Converts a string to upper case.
     *
     * @param input the input string
     */
    static std::basic_string<char> toUpper(const std::basic_string<char>& input);

    /**
     * Breaks the input text into lines of the same size avoiding splitting
     *  words at the end of the string using a maximum text occupancy with
     *  respect to the current terminal size.
     *
     *  @param input        the input string
     *  @param widthPercent the percentage occupancy with respect to screen width
     *  @return the list of wrapped strings
     */
    static std::list<std::string> fitText(const std::string& input, const short& widthPercent);

    /**
     * Breaks the input text into lines of the same size avoiding splitting
     * words at the end of the string.
     *
     * @param input the input string
     * @param width the limit
     *
     * @return the list of wrapped strings
     */
    static std::list<std::string> wrapText(const std::string& input, const std::size_t& width);

    /**
     * Retrieve current terminal size (rows x columns).
     *
     * @return the current terminal size.
     */
    static std::pair<std::size_t, std::size_t> getWindowSize();

   /**
    * Replaces first substring of a given input string with a replacement text.
    *
    * @param str the input string
    * @param from the text to search
    * @param to the replacement text
    * @return true if the operation succeeded, false otherwise
    * otherwise
    */
    static bool replace(std::string& str, const std::string& from, const std::string& to);

    /**
     * Replaces all substrings of a given input string with a replacement text.
     *
     * @param str the input string
     * @param from the text to search
     * @param to the replacement text
     * @return true if the operation succeeded, false otherwise
     * otherwise
     */
    static bool replaceAll(std::string& str, const std::string& from, const std::string& to);

    /**
     * Highlights all occurrences of a text in a given string
     *
     * @param str the input string
     * @param text the text to highlight
     * return the highlight version of the string
     */
    static std::string highlight(const std::string& str, const std::string& text);

    /**
     * Checks if a string consists only of whitespaces
     *
     * @param input the string to check
     * @return true if the string contains only whitespaces, false
     * otherwise
     */
    static bool isWhiteSpace(const std::string& input);

    /**
     * Checks if a string is empty or consists only of whitespaces.
     *
     * @param input the string to check
     * @return true if the string is empty or contains only whitespaces,
     * false otherwise
     */
    static bool isEmptyOrBlank(const std::string &input);

    /**
     * Left trims a given string effectively removing the trailing characters
     * supplied.
     *
     * @param str the input string
     * @param chars characters to remove
     * @return the string trimmed on its left side
     */
    static std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

    /**
    * Right trims a given string effectively removing the trailing characters
    * supplied.
    *
    * @param str the input string
    * @param chars characters to remove
    * @return the string trimmed on its right side
    */
    static std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

    /**
    * Trims a given string on both sides effectively removing the trailing characters
    * supplied.
    *
    * @param str the input string
    * @param chars characters to remove
    * @return the string trimmed
    */
    static std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ");

    /**
     * Converts a given string to the target variable using the
     * given type.
     *
     * @tparam T the target type
     * @param t the target
     * @param s the source string
     * @param f the conversion function (i.e. one of std::hex, std::dec or std::oct)
     * @return true if the operation succeeded, false otherwise
     */
    template <class T>
    static bool from_string(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
    {
        std::istringstream iss(s);
        return !(iss >> f >> t).fail();
    }

};

#endif //PROPS_STRING_UTILS_H
