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

#ifndef PROPS_PROPS_HELP_COMMAND_H
#define PROPS_PROPS_HELP_COMMAND_H

#include "props_cmd.h"
#include <sstream>


class PropsHelpCommand : public PropsCommand {

public:

    PropsHelpCommand() {
        id_          = "HELP";
        name_        = "help";
        args_        = { "<command>" };
        description_ = "Retrieve detailed options for the given command";
    }

    void parse(const int &argc, const char **argv) override;

    void execute(PropsResult &result) override;

private:

    std::string helpMessage_;
};

#endif //PROPS_PROPS_HELP_COMMAND_H
