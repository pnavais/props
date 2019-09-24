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

#include <iostream>
#include "props_cmd.h"
#include "props_cli.h"
#include "exec_exception.h"
#include "rang.hpp"

/**
 * Starts the parsing of the command line arguments extracting the
 * sub-command to execute.If available the command is then executed
 * with the corresponding displayed in the terminal.
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 * @return the exit code
 */
int main(int argc, char **argv)
{
    int ret_code = 0;
    PropsCommand* command = nullptr;

    try {
        command = PropsCLI::parse(argc,argv);
        if (command != nullptr) {
            command->run();
        }
    } catch (InitializationException& exception) {
        std::cerr << rang::fgB::yellow << exception.get_info() << rang::fg::reset << std::endl;
        if (command != nullptr) {
            command->getHelp();
        }
        ret_code = 1;
    } catch (ExecutionException& exception) {
        std::cerr << ((exception.getResult().getSeverity() == res::WARN) ? rang::fgB::yellow : rang::fgB::red)
                  << exception.get_info() << rang::fg::reset << std::endl;
        ret_code = 2;
    }

	return ret_code;
}
