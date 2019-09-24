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

#ifndef PROPS_PROPS_CLI_H
#define PROPS_PROPS_CLI_H

#include "props_cmd.h"
#include <iostream>

namespace p_cli {
    static const char DEFAULT_HELP_CMD_ID[]    = "HELP";
    static const char DEFAULT_VERSION_CMD_ID[] = "VERSION";
}

class PropsCLI {

public:

    /**
     * Process the command line arguments to retrieve
     * the props param holder.
     *
     * @param argc the number of arguments
     * @param argv the array of arguments
     * @return the props params holder
     */
    static PropsCommand* parse(const int& argc, char* argv[]);

};

#endif //PROPS_PROPS_CLI_H
