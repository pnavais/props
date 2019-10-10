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

#ifndef PROPS_UNKNOWN_COMMAND_H
#define PROPS_UNKNOWN_COMMAND_H

#include "props_cmd.h"
#include <sstream>

class PropsUnknownCommand : public PropsCommand {

public:

    explicit PropsUnknownCommand(const std::string& command) {
        command_     = command;
        id_ = name_  = "UNKNOWN";
        description_ = "Unknown command";
    }

    std::unique_ptr<PropsResult> execute() override;

private:

    std::string command_;

};

#endif //PROPS_UNKNOWN_COMMAND_H
