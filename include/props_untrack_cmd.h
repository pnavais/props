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

#ifndef PROPS_UNTRACK_COMMAND_H
#define PROPS_UNTRACK_COMMAND_H

#include "props_cmd.h"
#include "result.h"
#include <sstream>

namespace untrack_cmd {
    static const char* _UNTRACKED_FILE_  = "object";
    static const char* _ALIAS_FILE_      = "alias";
}

class PropsUntrackCommand : public PropsCommand {

public:

    /**
     * Constructor of the Untrack command
     */
    PropsUntrackCommand() {
        id_ = name_  = "untrack";
        tagLine_     = "Removes tracked files";
        description_ = "Removes a properties file from the list of tracked files (by default stored in $HOME/.config/props/props-tracker.conf)."
                       "In case the file does not exist a warning message is displayed."
                       "Additionally the command allows un-tracking files using a supplied alias.";

        args_ = { PropsArg::make_arg(untrack_cmd::_UNTRACKED_FILE_, "File/alias to untrack",
                  { PropsOption::make_opt(untrack_cmd::_ALIAS_FILE_, "Untrack by alias") }) };
    }

    /**
    * Parse the command line arguments to initialize the command.
    *
    * @param argc the number of arguments supplied
    * @param argv the array of arguments
    *
    */
    void parse(const int& argc, char* argv[]) noexcept(false) override;

    /**
     * Executes the command retrieving a result.
     *
     * @param result the result to be displayed
     */
    void execute(PropsResult &result) override;

    /**
     * Removes the selected filed from the list
     * of tracked files.
     *
     * @param out the output stream
     * @return the result of the operation
     */
    Result untrackFile(std::ostringstream& out);

};

#endif //PROPS_UNTRACK_COMMAND_H
