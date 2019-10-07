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

#ifndef PROPS_PROPS_RESULT_H
#define PROPS_PROPS_RESULT_H

#include <ostream>
#include "result.h"

class PropsResult {

public:

    PropsResult() = default;

    virtual ~PropsResult() = default;

    /**
     * Formats the contents of the result in an
     * output stream.
     *
     * @param out the stream with formatted result.
     */
    virtual void format(std::ostream& out) const {
        out << output_;
    };

    /**
     * Retrieves the description message
     *
     * @return the execution result
     */
    const std::string &getOutput() const {
        return output_;
    }

    /**
     * Sets the execution result
     *
     * @param result the execution result
     */
    void setOutput(const std::string &output) {
        output_ = output;
    }

    /**
     * Retrieves the description message
     *
     * @return the execution result
     */
    const Result& getExecResult() const {
        return result_;
    }

    /**
     * Sets the execution result
     *
     * @param result the execution result
     */
    void setResult(const Result& result) {
        result_ = result;
    }

protected:

    std::string output_;
    Result result_{res::VALID};

};

#endif //PROPS_PROPS_RESULT_H