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

#include "string_utils.h"
#include "config_static.h"
#include <sstream>
#include <iomanip>


#if defined(IS_LINUX) || defined(IS_MAC)
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#endif

namespace str_utils {
    const int DEFAULT_MAX_WIDTH = 120;
}

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
std::basic_string<char> StringUtils::toUpper(const std::basic_string<char>& input)
{
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
std::list<std::string> StringUtils::fitText(const std::string& input, const short& widthPercent)
{
    std::pair<std::size_t, std::size_t> winSize = getWindowSize();
    std::size_t cols = winSize.second;

    auto w = (widthPercent > 100) ? 100 : widthPercent;
    size_t width = ((cols * w) / 100);
    width = (width <= 0) ? str_utils::DEFAULT_MAX_WIDTH : width;

    return StringUtils::wrapText(input, width);
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
std::list<std::string> StringUtils::wrapText(const std::string& input, const std::size_t& width)
{
    std::list<std::string> lines;

    std::string tmp;
    std::ostringstream out;

    for (auto c : input) {
        if (!isspace(c)) {
            tmp += c;
        } else {
            // Cannot append current word to the stream (exceeds)
            if ((tmp.size() + out.str().size() + 1) > width) {
                // Flush current stream
                lines.push_back(out.str());
                out.str("");
                out.clear();
            }

            // Append whitespace + word
            out << tmp << c;
            tmp.clear();
        }
    }

    // Flush final word (if any)
    out << tmp;
    lines.push_back(out.str());

    return lines;
}


/**
  * Retrieve current terminal size (rows x columns).
  *
  * @return the current terminal size.
  */
std::pair<std::size_t, std::size_t> StringUtils::getWindowSize()
{
    std::size_t rows= 0;
    std::size_t cols = 0;

#if defined(IS_LINUX) || defined(IS_MAC)
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    rows = w.ws_row;
    cols = w.ws_col;
#endif

    return std::make_pair(rows, cols);

}