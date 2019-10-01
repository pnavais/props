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

#ifndef PROPS_TRACKER_COMMAND_H
#define PROPS_TRACKER_COMMAND_H

#include "props_cmd.h"
#include "props_tracker.h"
#include "props_tracker_factory.h"
#include <sstream>

namespace tracker_cmd {
    const char* const _TRACKED_FILE_        = "file";
    const char* const _ALIAS_FILE_          = "alias";
    const char* const _MASTER_FILE_         = "master";
    const char* const _TRACKER_ADD_CMD_     = "add";
    const char* const _TRACKER_LS_CMD_      = "ls";
    const char* const _TRACKER_UNALIAS_CMD_ = "unalias";
    const char* const _TRACKER_UNTRACK_CMD_ = "untrack";
}

class PropsTrackerCommand : public PropsCommand {

public:

    /**
     * Constructor of the Track command
     */
    PropsTrackerCommand() {
        id_ = name_  = "tracker";
        tagLine_     = "Add | List tracked files";
        description_ = "Adds a properties file to the list of tracked files (by default the list is stored physically in $HOME/.config/props/props-tracker.conf)."
                       "In case the file does not exist a warning message is displayed and the file is ignored.Once added to the list of tracked files "
                       ", the file will be queried in case global option is active.In case the tracked file is flagged as master, "
                       "the file would be used as main source in case global option is disabled."
                       "Additionally this command allows listing all "
                       "currently tracked properties files.";

        args_ = { PropsArg::make_cmd(tracker_cmd::_TRACKER_ADD_CMD_, { "<file>" } , "Adds the file to the tracker",
                           { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "Sets an alias for the file", {"<name>"}),
                             PropsOption::make_opt(tracker_cmd::_MASTER_FILE_, "Sets the file as master") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_LS_CMD_ , "List all tracked files"),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_UNALIAS_CMD_ , { "<file>" } , "Removes a given alias"),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_UNTRACK_CMD_ , { "<file>" } , "Removes a given file from the tracker") };
        propsTracker_ = &PropsTrackerFactory::getDefaultTracker();
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

private:

    /**
     * The property tracker
     */
    PropsTracker* propsTracker_;

};

#endif //PROPS_TRACKER_COMMAND_H
