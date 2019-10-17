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

#include "string_utils.h"
#include "config_static.h"
#include "rang.hpp"
#include <sstream>

#if defined(IS_LINUX) || defined(IS_MAC)

#include <sys/ioctl.h>
#include <unistd.h>
#include <pcrecpp.h>

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
std::string StringUtils::padding(const std::string &input, const std::string &pattern, const std::size_t &maxLength) {
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
std::string StringUtils::padding(const std::string &input, const std::size_t &maxLength) {
    return StringUtils::padding(input, DEFAULT_PATTERN, maxLength);
}

/**
 * Repeats the input pattern the given number
 * of times.
 *
 * @param input the input pattern
 * @param nTimes the number of repetitions
 * @return the expanded string
 */
std::string StringUtils::expand(const std::string& input, const std::size_t& nTimes) {
    std::ostringstream out;
    size_t counter = 0;
    while (counter < nTimes) {
        out << input;
        counter++;
    }
    return out.str();
}

/**
 * Transforms a list of string values to a flat string
 * using the provided separator.
 *
 * @param args the list of arguments
 * @return
 */
std::string StringUtils::toFlatString(const std::list<std::string> &args, const std::string &separator) {
    std::ostringstream out;

    std::string prefix;
    for (auto &arg : args) {
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
std::basic_string<char> StringUtils::toUpper(const std::basic_string<char> &input) {
    std::basic_string<char> output = input;
    for (auto &p : output) {
        p = static_cast<char>(toupper(p));
    }

    return output;
}

/**
 * Converts a string to lower case.
 *
 * @param input the input string
 */
std::basic_string<char> StringUtils::toLower(const std::basic_string<char> &input) {
    std::basic_string<char> output = input;
    for (auto &p : output) {
        p = static_cast<char>(tolower(p));
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
std::list<std::string> StringUtils::fitText(const std::string &input, const short &widthPercent) {
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
std::list<std::string> StringUtils::wrapText(const std::string &input, const std::size_t &width) {
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
std::pair<std::size_t, std::size_t> StringUtils::getWindowSize() {
    std::size_t rows = 0;
    std::size_t cols = 0;

#if defined(IS_LINUX) || defined(IS_MAC)
    struct winsize w{};
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    rows = w.ws_row;
    cols = w.ws_col;
#endif

    return std::make_pair(rows, cols);
}

/**
 * Replaces first substring of a given input string with a replacement text.
 *
 * @param str the input string
 * @param from the text to search
 * @param to the replacement text
 * @param caseSensitive enable/disable case sensitivity
 * @return true if the operation succeeded, false otherwise
 * otherwise
 */
bool StringUtils::replace(std::string& str, const std::string& from, const std::string& to, const bool& caseSensitive) {
    size_t start_pos = (caseSensitive) ? str.find(from) : find_ci(str, from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

/**
 * Replaces all substrings of a given input string with a replacement text.
 *
 * @param str the input string
 * @param from the text to search
 * @param to the replacement text
 * @param caseSensitive enable/disable case sensitivity
 * @return true if the operation succeeded, false otherwise
 * otherwise
 */
bool StringUtils::replaceAll(std::string& str, const std::string& from, const std::string& to, const bool& caseSensitive) {
    if(from.empty())
        return false;
    size_t start_pos = 0;
    while((start_pos = ((caseSensitive) ? str.find(from, start_pos) : find_ci(str, from, start_pos))) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }

    return true;
}

/**
 * Find Case Insensitive Sub String in a given substring.
 *
 * @param input the input data
 * @param text the text to search
 * @param pos the position to start from
 */
size_t StringUtils::find_ci(std::string input, std::string text, size_t pos) {
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    std::transform(text.begin(), text.end(), text.begin(), ::tolower);

    return input.find(text, pos);
}


/**
 * Highlights all occurrences of a text in a given string
 *
 * @param str the input string
 * @param replStr the text to highlight
 * @param pos position of the match
 * return the highlight version of the string
 */
std::string StringUtils::highlight(const std::string& str, const std::string& replStr, const size_t& pos) {
    std::ostringstream out;

    rang::setControlMode(rang::control::Force);
    out << rang::style::reversed << rang::fgB::yellow << replStr << rang::style::reset;
    rang::setControlMode(rang::control::Auto);
    std::string hlStr = str;

    hlStr.replace(pos, replStr.size(), out.str());

    return hlStr;
}

/**
 * Checks if a string consists only of whitespaces
 *
 * @param input the string to check
 * @return true if the string contains only whitespaces, false
 * otherwise
 */
bool StringUtils::isWhiteSpace(const std::string &input) {
    return (input.find_first_not_of(' ') == std::string::npos);
}

/**
 * Checks if a string is empty or consists only of whitespaces.
 *
 * @param input the string to check
 * @return true if the string is empty or contains only whitespaces,
 * false otherwise
 */
bool StringUtils::isEmptyOrBlank(const std::string &input) {
    return (input.empty() || (input.find_first_not_of(' ') == std::string::npos));
}

/**
 * Left trims a given string effectively removing the trailing characters
 * supplied.
 *
 * @param str the input string
 * @param chars characters to remove
 * @return the string trimmed on its left side
 */
std::string& StringUtils::ltrim(std::string &str, const std::string &chars) {
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

/**
 * Right trims a given string effectively removing the trailing characters
 * supplied.
 *
 * @param str the input string
 * @param chars characters to remove
 * @return the string trimmed on its right side
 */
std::string& StringUtils::rtrim(std::string &str, const std::string &chars) {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

/**
 * Trims a given string on both sides effectively removing the trailing characters
 * supplied.
 *
 * @param str the input string
 * @param chars characters to remove
 * @return the string trimmed
 */
std::string& StringUtils::trim(std::string &str, const std::string &chars) {
    return ltrim(rtrim(str, chars), chars);
}

/**
 * Splits a given command line string into an array of arguments.
 *
 * @param cmdline the command line
 * @param argv the array of arguments
 * @return the number of arguments or error code if negative
 */
int StringUtils::split_cmdline(const std::string& inputStr, char **& argv) {
    int src, dst;
    char quoted = 0;

    std::list<char *> argList;

    char* cmdline = new char[inputStr.size() + 1];
    strcpy(cmdline, inputStr.c_str());
    argList.push_back(cmdline);

    for (src = dst = 0; cmdline[src];) {
        char c = cmdline[src];
        if (!quoted && isspace(c)) {
            cmdline[dst++] = 0;
            while (cmdline[++src]
                   && isspace(cmdline[src])); /* skip */
            argList.push_back(cmdline + dst);
        } else if (!quoted && (c == '\'' || c == '"')) {
            quoted = c;
            src++;
        } else if (c == quoted) {
            quoted = 0;
            src++;
        } else {
            if (c == '\\' && quoted != '\'') {
                src++;
                c = cmdline[src];
                if (!c) {
                    return -SPLIT_CMDLINE_BAD_ENDING;
                }
            }
            cmdline[dst++] = c;
            src++;
        }
    }

    cmdline[dst] = 0;

    if (quoted) {
        return -SPLIT_CMDLINE_UNCLOSED_QUOTE;
    }

    argv = new char*[argList.size()];
    int counter = 0;
    for (auto& str : argList) {
        argv[counter] = new char[sizeof(str)+1];
        strcpy(argv[counter], str);
        counter++;
    }

    delete [] cmdline;

    return counter;
}