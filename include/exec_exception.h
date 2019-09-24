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

#ifndef PROPS_EXEC_EXCEPTION_H
#define PROPS_EXEC_EXCEPTION_H

#include <iostream>
#include <exception>
#include <utility>
#include "result.h"

/**
 * A simple exception raised during execution detected anomalies
 */
class ExecutionException : public std::runtime_error {

public:

    explicit ExecutionException(const char *info) : runtime_error(info), info_(info), result_(res::ERROR) {}
    explicit ExecutionException(std::string info) : runtime_error(info), info_(std::move(info)), result_(res::ERROR) {}
    explicit ExecutionException(Result res) : runtime_error(res.getMessage()), info_(res.getMessage()), result_(std::move(res)) {}

    /**
     * Retrieves the short description message.
     *
     * @return the description message
     */
    const char* what() const noexcept override {
        return "Error during execution: ";
    }

    /**
     * Retrieves the detailed description.
     *
     * @return the detailed description
     */
    const char* get_info() const { return info_.c_str(); }

    /**
     * Sets the optional result.
     *
     * @param res the result
     */
    void setResult(const Result& res) {
        result_ = res;
    }

    /**
     * Retrieves the optional result.
     *
     * @return the result
     */
    const Result& getResult() const {
        return result_;
    }

private:

    std::string info_;
    Result result_;


};

#endif //PROPS_EXEC_EXCEPTION_H
