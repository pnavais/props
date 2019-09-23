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

#ifndef PROPS_RESULT_H
#define PROPS_RESULT_H

#include <string>

namespace res {
    const static bool VALID = true;
    const static bool ERROR = false;
}

/**
 * Models the result of a given operation holding the validity status
 * and an optional message.
 */
class Result {
public:

    /**
     * Avoids instantiation
     */
    Result() = delete;

    /**
     * Constructor with validity flag and message
     * @param validity the validity flag
     * @param message the message
     */
    Result(bool validity, const std::string &message) {
        validity_ = validity;
        message_ = message;
    }

    /**
    * Constructor with validity flag and message
    * @param validity the validity flag
    * @param message the message
    */
    explicit Result(bool validity) {
        validity_ = validity;
    }

    /**
     * Creates an error using the given message.
     *
     * @param message the message
     * @return the error result
     */
    static Result make_error(const std::string &message) {
        Result result{false, message};
        return result;
    }

    /**
     * Creates a valid result using the given message.
     *
     * @param message the message
     * @return the error result
     */
    static Result make_valid(const std::string &message) {
        Result result{true, message};
        return result;
    }

    /**
     * Retrieves the message.
     *
     * @return the message
     */
    const std::string &getMessage() const {
        return message_;
    }

    /**
     * Sets the message.
     *
     * @param message the message
     */
    void setMessage(const std::string &message) {
        message_ = message;
    }

    /**
     * The validity flag.
     *
     * @return the validity flag
     */
    bool isValid() {
        return validity_;
    }

    /**
     * Compares the result using only the
     * validity flag.
     *
     * @param rhs the result to compare
     * @return true if same validity, false
     * otherwise
     */
    bool operator==(const Result &rhs) const {
        return validity_ == rhs.validity_;
    }

    /**
     * Compares the result using only the
     * validity flag.
     *
     * @param rhs the result to compare
     * @return true if not same validity, false
     * otherwise
     */
    bool operator!=(const Result &rhs) const {
        return rhs.validity_ != this->validity_;
    }

    /**
    * Compares the result using only the
    * validity flag.
    *
    * @param rhs the result to compare
    * @return true if not same validity, false
    * otherwise
    */
    bool operator!=(const bool &rhs) const {
        return rhs != this->validity_;
    }

    /**
     * Negates the validity of the current
     * result.
     * @return the validity opposite
     */
    bool operator!() {
        return !(this->validity_);
    }

    /**
     * Assignment operator from a given validity.
     *
     * @param validity the validity
     * @return the result
     */
    Result &operator=(const bool &validity) {
        validity_ = validity;
        return *this;
    }

private:
    bool validity_;       // The validity flag
    std::string message_; // The description message
};

#endif //PROPS_RESULT_H
