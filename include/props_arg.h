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

#ifndef PROPS_PROPS_ARG_H
#define PROPS_PROPS_ARG_H

#include <string>
#include <list>
#include "props_option.h"

class PropsArg {
public:

    static PropsArg make_cmd(const std::string& name, const std::string& desc = "", const std::list<PropsOption>& options = {}) {
        return make_full_arg(name, desc, false, options);
    }

    static PropsArg make_arg(const std::string& name, const std::string& desc = "", const std::list<PropsOption>& options = {}) {
        return make_full_arg(name, desc, true, options);
    }

    const std::string& getName() const {
        return name_;
    }

    void setName(const std::string& name) {
        name_ = name;
    }

    const char& getShortName() const {
        return shortName_;
    }

    void setShortName(const char& shortName) {
        shortName_ = shortName;
    }

    const std::string& getDescription() const {
        return description_;
    }

    void setDescription(const std::string& description) {
        description_ = description;
    }

    const std::list<PropsOption>& getOptions() const {
        return options_;
    }

    void setOptions(const std::list<PropsOption>& options) {
        options_ = options;
    }

    void setAnonymous(const bool anonymous) {
        anonymous_ = anonymous;
    }

    bool isAnonymous() const {
        return anonymous_;
    }

    bool operator==(const PropsArg &rhs) const {
        return name_ == rhs.name_;
    }

    bool operator!=(const PropsArg &rhs) const {
        return !(rhs == *this);
    }

private:

    static PropsArg make_full_arg(const std::string& name, const std::string& desc = "", const bool& anonymous = false, const std::list<PropsOption>& options = {}) {
        PropsArg arg;
        arg.setName(name);
        arg.setShortName(name.at(0));
        arg.setDescription(desc);
        arg.setOptions(options);
        arg.setAnonymous(anonymous);
        return arg;
    }

    std::string name_;
    char shortName_{};
    std::string description_;
    std::list<PropsOption> options_;
    bool anonymous_{false};

};

#endif //PROPS_PROPS_ARG_H
