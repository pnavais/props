#include <utility>
#include <string>

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

#ifndef PROPS_SEARCH_OPTIONS_H
#define PROPS_SEARCH_OPTIONS_H

#include <string>
#include "generic_options.h"

/**
 * A simple holder for search options
 */
class PropsSearchOptions {

public:
    PropsSearchOptions() = default;

    /**
     * Retrieve the search key.
     *
     * @return the search key
     */
    const std::string &getKey() const {
        return key_;
    }

    /**
     * Sets the search key
     *
     * @param key the search key
     */
    void setKey(const std::string &key) {
        key_ = key;
    }

    /**
     * Retrieve the case sensitive option
     *
     * @return the case sensitive option
     */
    global_options::Opt getCaseSensitive() const {
        return caseSensitive_;
    }

    /**
     * Sets the case sensitive option
     *
     * @param caseSensitive case sensitive option
     */
    void setCaseSensitive(global_options::Opt caseSensitive) {
        caseSensitive_ = caseSensitive;
    }

    /**
     * Retrieves the key/value separator
     *
     * @return the key/value separator
     */
    const std::string &getSeparator() const {
        return separator_;
    }

    /**
     * Sets the key/value separator
     *
     * @param separator the key/value separator
     */
    void setSeparator(const std::string &separator) {
        separator_ = separator;
    }

    /**
     * Retrieves the replacement string
     *
     * @return the replacement string
     */
    const std::string &getReplacement() const {
        return replacement_;
    }

    /**
     * Sets the replacement string
     *
     * @param replacement the replacement string
     */
    void setReplacement(const std::string &replacement) {
        replacement_ = replacement;
    }

    /**
     * Retrieves the partial match option
     *
     * @return the partial match option
     */
    global_options::Opt getPartialMatch() const {
        return partialMatch_;
    }

    /**
     * Sets the partial match option
     *
     * @param partialMatch the partial match option
     */
    void setPartialMatch(global_options::Opt partialMatch) {
        partialMatch_ = partialMatch;
    }

    /**
     * Checks whether the match shall be made
     * on values or not.
     *
     * @return the flag controlling the match type.
     */
    bool isMatchValue() const {
        return matchValue_;
    }

    /**
     * Sets whether the match shall be made
     * on values or not.
     *
     * @param matchValue the flag controlling the match type
     */
    void setMatchValue(bool matchValue) {
        matchValue_ = matchValue;
    }

    /**
     * Checks whether the term is expressed
     * as a regular expresion or not.
     *
     * @return true if expressed as regex, false otherwise
     */
    bool isRegex() const {
        return isRegex_;
    }

    /**
     * Sets whether the term is expressed
     * as a regular expression or not.
     *
     * @param isRegex the flag to control the term type
     */
    void setIsRegex(bool isRegex) {
        isRegex_ = isRegex;
    }

    /**
     * Checks whether the search involves
     * a value replacement or not.
     *
     * @return true if replacement requested, false otherwise
     */
    bool isReplace() const {
        return replace_;
    }

    /**
     * Sets whether the search must perform a modification
     * as well.
     *
     * @param replace true to replace, false otherwise
     */
    void setReplace(bool replace) {
        replace_ = replace;
    }

private:

    std::string key_;
    global_options::Opt caseSensitive_;
    std::string separator_;
    std::string replacement_;
    global_options::Opt partialMatch_;
    bool matchValue_;
    bool isRegex_;
    bool replace_;

};

#endif //PROPS_SEARCH_OPTIONS_H
