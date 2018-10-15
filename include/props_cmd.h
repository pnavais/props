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

#ifndef PROPS_PROPS_COMMAND_H
#define PROPS_PROPS_COMMAND_H

#include "init_exception.h"
#include "props_search_result.h"
#include "config.h"
#include <string>

class PropsCommand {

public:

    virtual ~PropsCommand() = default;

    /**
     * Retrieves the identifier.
     *
     * @return the identifier
     */
    const std::string &getId() const {
        return id_;
    }

    /**
     * Sets the identifier
     *
     * @param id the identifier to set
     */
    void setId(const std::string &id) {
        id_ = id;
    }

    /**
     * Retrieves the command name.
     *
     * @return the command name
     */
    const std::string &getName() const {
        return name_;
    }

    /**
     * Retrieves the command allowed arguments.
     *
     * @return the list of allowed arguments
     */
    const std::list<std::string> &getArgs() const {
        return args_;
    }

    /**
     * Retrieves the command description.
     *
     * @return the command description
     */
    const std::string &getDescription() const {
        return description_;
    }

    /**
     * Display a help message
     */
    virtual void getHelp() {
        std::cout << "\nExpected syntax : " << PACKAGE_NAME << " " << name_ << " ";
        std::string prefix;
        for (auto& arg : args_) {
            std::cout << prefix << arg;
            prefix = " ";
        }
        std::cout << std::endl;
    }

    /**
     * Parse the command line arguments to initialize the command.
     *
     * @param argc the number of arguments supplied
     * @param argv the array of arguments
     *
     */
    virtual void parse(const int& argc, const char** argv) noexcept(false) {
        (void)argc;
        (void)argv;
    };

    /**
     * Executes the command and provides with a
     * result.
     *
     * @param result the result of the command execution
     */
    virtual void execute(PropsResult& result) noexcept(false) = 0;

    /**
     * Executes the command and formats its results
     * using the standard output.
     */
    virtual void run() noexcept(false) {
        PropsResult result;
        execute(result);
        result.format(std::cout);
    }

protected:

    std::string id_;
    std::string name_;
    std::list<std::string> args_;
    std::string description_;

};

#endif //PROPS_PROPS_COMMAND_H
