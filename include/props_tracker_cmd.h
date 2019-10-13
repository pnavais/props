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

namespace tracker_cmd {
    const char* const _TRACKED_FILE_             = "file";
    const char* const _ALIAS_FILE_               = "alias";
    const char* const _MASTER_FILE_              = "master";
    const char* const _GROUP_NAME_               = "group";
    const char* const  _FORCE_MOVE_              = "force";
    const char* const _OLD_GROUP_NAME_           = "old_group";
    const char* const _TRACKER_ADD_CMD_          = "add";
    const char* const _TRACKER_LS_CMD_           = "ls";
    const char* const _TRACKER_UNALIAS_CMD_      = "unalias";
    const char* const _TRACKER_UNTRACK_CMD_      = "untrack";
    const char* const _TRACKER_UNGROUP_CMD_      = "ungroup";
    const char* const _TRACKER_RENAME_GROUP_CMD_ = "rename-group";
    const char* const _TRACKER_GROUP_CMD_        = "group";
    const char* const _TRACKER_SET_MASTER_CMD_   = "set-master";
    const char* const _TRACKER_SET_ALIAS_CMD_    = "set-alias";
    const char* const _TRACKER_CLEAR_CMD_        = "clear";
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

        args_ = { PropsArg::make_cmd(tracker_cmd::_TRACKER_ADD_CMD_, { "<file...>" } , "Adds the file(s) to the tracker",
                           { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "Sets an alias for the file", {"<name>"}),
                             PropsOption::make_opt(tracker_cmd::_MASTER_FILE_, "Sets the file as master"),
                             PropsOption::make_opt(tracker_cmd::_GROUP_NAME_, "Sets a group for the file", {"<name>"})}),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_LS_CMD_ , "List all tracked files"),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_CLEAR_CMD_ , "Removes all tracked files"),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_SET_MASTER_CMD_ , { "<file|alias>" } , "Sets the file as master",
                           { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "the name specified is an alias") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_SET_ALIAS_CMD_, { "<file>" } , "Sets an alias for the tracked file",
                                     { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "The alias for the file", {"<name>"}) }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_UNALIAS_CMD_ , { "<file|alias>" } , "Removes a given alias from a file",
                                     { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "the name specified is an alias") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_UNTRACK_CMD_ , { "<file|alias>" } , "Removes a given file from the tracker",
                                     { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "the name specified is an alias") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_GROUP_CMD_, { "<file|alias>", "<group>" } , "Moves file to the group",
                                     { PropsOption::make_opt(tracker_cmd::_ALIAS_FILE_, "the name specified is an alias") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_UNGROUP_CMD_, { "<group>" } , "Removes a given group moving all its contained files to the default group",
                                     { PropsOption::make_opt(tracker_cmd::_TRACKER_UNTRACK_CMD_, "Untracks all files in the group") }),
                  PropsArg::make_cmd(tracker_cmd::_TRACKER_RENAME_GROUP_CMD_, { "<old_group>", "<new_group>" } , "Changes the name of an existing group",
                                     { PropsOption::make_opt(tracker_cmd::_FORCE_MOVE_, "move to new group event if already existing") })};
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
    std::unique_ptr<PropsResult> execute() override;


private:

    /**
     * Tracks the selected file.
     *
     * @return the result of the operation
     */
    Result trackFile();

    /**
     * Tracks the selected files.
     *
     * @return the result of the operation
     */
    Result trackFiles();

    /**
     * Removes from the tracker the selected filed using the file
     * name or the alias (if set).
     *
     * @return the result of the operation
     */
    Result untrackFile();

    /**
     * Sets the given file/alias as the new master.
     *
     * @return the result of the operation
     */
    Result setMaster();

    /**
     * Sets an alias for a given file.
     *
     * @return the result of the operation
     */
    Result setAlias();

    /**
     * Removes an alias from a given file.
     *
     * @return the result of the operation
     */
    Result unalias();

    /**
     * Moves a file to a group from the tracker.
     * If the group does no exist yet it is
     * effectively created.
     *
     * @return the result of the operation
     */
    Result group();

    /**
     * Removes an a group from the tracker, effectively
     * moving all its contained files to the default
     * group.
     *
     * @return the result of the operation
     */
    Result ungroup();

    /**
     * Renames an existing group from the tracker.
     * effectively moving all its contained files to the target
     * group.
     *
     * @return the result of the operation
     */
    Result renameGroup();

    /**
     * The property tracker
     */
    PropsTracker* propsTracker_{nullptr};

};

#endif //PROPS_TRACKER_COMMAND_H
