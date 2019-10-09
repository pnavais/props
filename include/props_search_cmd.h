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

#ifndef PROPS_SEARCH_COMMAND_H
#define PROPS_SEARCH_COMMAND_H

#include "props_cmd.h"
#include "props_tracker.h"

namespace search_cmd {
    const char* const _ALIAS_FILE_    = "alias";
    const char* const _SEARCH_VALUE_  = "value";
    const char* const _GROUP_SEARCH_  = "group";
    const char* const _GLOBAL_SEARCH_ = "global";
    const char* const _USE_REGEX_     = "expression";
    const char* const _SEARCH_CMD_    = "search";
}

class PropsSearchCommand : public PropsCommand {

public:

    /**
     * Constructor of the Search command
     */
    PropsSearchCommand() {
        id_ = name_  = "search";
        tagLine_     = "Search key/values in a file or list of tracked files";
        description_ = "Tries to match a given regex/wildcard expression in keys or values of specific files "
                       "or the list of currently tracked files if no file is supplied."
                       "In case no options are specified, the master file of the tracker is the default file to lookup but "
                       "all tracked files can be queried simultaneously if a global search is performed. It is also possible "
                       "to query files present in groups, or files using aliases.";

        args_ = { PropsArg::make_cmd(search_cmd::_SEARCH_CMD_, { "<term>", "[files...]" } , "Searches the files for a given key/value",
                                     { PropsOption::make_opt(search_cmd::_ALIAS_FILE_, "Searches the file using the alias", {"<alias>"}),
                                       PropsOption::make_opt(search_cmd::_SEARCH_VALUE_, "Perform a search by value"),
                                       PropsOption::make_opt(search_cmd::_USE_REGEX_, "The term is expressed as a regular expression"),
                                       PropsOption::make_opt(search_cmd::_GLOBAL_SEARCH_, "Perform a global search"),
                                       PropsOption::make_opt(search_cmd::_GROUP_SEARCH_, "Perform a search group", {"<group_name>"})}) };
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
    PropsResult execute() override;


private:

    /**
     * Perform a search using the supplied options.
     *
     * @return the search results
     */
     PropsSearchResult search();

    /**
     * The property tracker
     */
    PropsTracker* propsTracker_{nullptr};

};

#endif //PROPS_SEARCH_COMMAND_H