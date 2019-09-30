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

#ifndef PROPS_TRACK_COMMAND_H
#define PROPS_TRACK_COMMAND_H

#include "props_cmd.h"
#include <sstream>

namespace track_cmd {
    static const char* _TRACKED_FILE_        = "file";
    static const char* _ALIAS_FILE_          = "alias";
    static const char* _MASTER_FILE_         = "master";
    static const char* _TRACKER_ADD_CMD_     = "add";
    static const char* _TRACKER_LS_CMD_      = "ls";
    static const char* _TRACKER_UNALIAS_CMD_ = "unalias";
}

class PropsTrackCommand : public PropsCommand {

public:

    /**
     * Constructor of the Track command
     */
    PropsTrackCommand() {
        id_ = name_  = "tracker";
        tagLine_     = "Add | List tracked files";
        description_ = "Adds a properties file to the list of tracked files (by default the list is stored physically in $HOME/.config/props/props-tracker.conf)."
                       "In case the file does not exist a warning message is displayed and the file is ignored.Once added to the list of tracked files "
                       ", the file will be queried in case global option is active.In case the tracked file is flagged as master, "
                       "the file would be used as main source in case global option is disabled."
                       "Additionally this command allows listing all "
                       "currently tracked properties files.";

        args_ = { PropsArg::make_cmd(track_cmd::_TRACKER_ADD_CMD_, "File to track",
                           { PropsOption::make_opt(track_cmd::_ALIAS_FILE_, "Sets an alias for the file", {"<name>"}),
                             PropsOption::make_opt(track_cmd::_MASTER_FILE_, "Sets the file as master") }),
                  PropsArg::make_cmd(track_cmd::_TRACKER_LS_CMD_ , "List all tracked files"),
                  PropsArg::make_cmd(track_cmd::_TRACKER_UNALIAS_CMD_ , "Removes a given alias") };
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
     * Tracks the selected filed.
     *
     * @param out the output stream
     * @return the result of the operation
     */
    Result trackFile(std::ostringstream& out);

};

#endif //PROPS_TRACK_COMMAND_H
