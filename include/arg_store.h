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

#ifndef ARG_STORE_H
#define ARG_STORE_H

#include <map>

/**
 * Allows storing the input arguments and the processed options.
 */
class ArgStore {
public:

    /**
     * Default constructor
     */
    ArgStore() {
        argc_ = -1;
        argv_ = nullptr;
    }

    /**
     * Creates an initial store for the given input arguments.
     *
     * @param argc the number of arguments
     * @param argv the list of arguments
     * @return the arguments store
     */
    static ArgStore make_store(const int& argc, char* argv[]) {
        ArgStore argStore;
        argStore.setArgc(argc);
        argStore.setArgv(argv);
        return argStore;
    }

    /**
     * Retrieve the number of arguments
     *
     * @return the number of arguments
     */
    const int& getArgc() const {
        return argc_;
    }

    /**
     * Retrieves the list of arguments.
     *
     * @return the list of arguments
     */
    char* const * getArgv() const {
        return argv_;
    }

    /**
     * Retrieves parsed the option map.
     *
     * @return the option map
     */
    const std::map<std::string, std::string> &getOptions() const {
        return options_;
    }

    /**
     * Stores an option identified by the given key and value
     * in the map.
     *
     * @param key the option key
     * @param value the option value
     */
    void addOption(const std::string& key, const std::string& value) {
        options_[key] = value;
    }

    /**
     * Retrieves the list of non-option arguments i.e.
     * the arguments not bound to a given option flag.
     *
     * @return the list of non-option arguments
     */
    const std::list<std::string> &getArgs() const {
        return args_;
    }

    /**
     * Adds a non-option argument to the list.
     *
     * @param arg the non-option argument
     */
    void addArg(const std::string& arg) {
        args_.push_back(arg);
    }

    /**
     * Retrieves the name of the matched command.
     *
     * @return the name of the matched command
     */
    const std::string &getCmdName() const {
        return cmdName_;
    }

    /**
     * Sets the name of the matched command.
     *
     * @param cmdName the name of the matched command
     */
    void setCmdName(const std::string &cmdName) {
        cmdName_ = cmdName;
    }

private:

    /**
     * Sets the CLI arguments count.
     *
     * @param argc the number of CLI arguments
     */
    void setArgc(const int& argc) {
        argc_ = argc;
    }

    /**
     * Sets the CLI arguments list.
     *
     * @param argv the arguments list
     */
    void setArgv(char* argv[]) {
        argv_ = argv;
    }

    int argc_;
    char** argv_;
    std::list<std::string> args_;
    std::map<std::string, std::string> options_;
    std::string cmdName_;
};

#endif //ARG_STORE_H
