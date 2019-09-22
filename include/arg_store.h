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

#ifndef ARG_STORE_H
#define ARG_STORE_H

/**
 * Allows storing the input arguments and the processed options.
 */
class ArgStore {
public:

    static ArgStore make_store(const int& argc, char* argv[]) {
        ArgStore argStore;
        argStore.setArgc(argc);
        argStore.setArgv(argv);
        return argStore;
    }

    const int& getArgc() const {
        return argc_;
    }

    char* const * getArgv() const {
        return argv_;
    }

    const std::map<std::string, void *> &getOptions() const {
        return options_;
    }

    void setOptions(const std::map<std::string, void *> &optionMap) {
        ArgStore::options_ = optionMap;
    }

    const std::list<std::string> &getArgs() const {
        return args_;
    }

    void setArgs(const std::list<std::string> &argList) {
        ArgStore::args_ = argList;
    }

    void addOption(const std::string& key, void* value) {
        options_[key] = value;
    }

    void addArg(const std::string& arg) {
        args_.push_back(arg);
    }


private:

    void setArgc(const int& argc) {
        argc_ = argc;
    }

    void setArgv(char* argv[]) {
        argv_ = argv;
    }

    int argc_;
    char** argv_;
    std::map<std::string, void*> options_;
    std::list<std::string> args_;

};

#endif //ARG_STORE_H
