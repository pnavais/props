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

#ifndef PROPS_OPT_H
#define PROPS_OPT_H

#include <string>
#include <list>

class PropsOption {
public:

    static PropsOption make_opt(const std::string& name, const std::string& desc = "", const std::list<std::string>& cmdList = {}) {
        PropsOption option;
        option.setName(name);
        option.setShortName(name.at(0));
        option.setDescription(desc);
        option.setCmdList(cmdList);
        return option;
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

    const std::list<std::string>& getCmdList() const {
        return cmdList_;
    }

    void setCmdList(const std::list<std::string>& cmdList) {
        cmdList_ = cmdList;
    }

private:

    std::string name_;
    char shortName_{};
    std::string description_;
    std::list<std::string> cmdList_;

};

#endif //PROPS_OPT_H
