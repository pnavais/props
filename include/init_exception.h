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

#ifndef PROPS_INIT_EXCEPTION_H
#define PROPS_INIT_EXCEPTION_H

#include <iostream>
#include <exception>

class InitializationException : public std::exception {

public:

    explicit InitializationException(char *info) : info_(info) {}

    const char* what () const noexcept override {
        return (("Error during initialization: ") + std::string(info_)).c_str();
    }

    const char* get_info() const { return info_; }

private:

    char* info_;


};

#endif //PROPS_INIT_EXCEPTION_H
