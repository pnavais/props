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

#ifndef PROPS_EDIT_COMMAND_H
#define PROPS_EDIT_COMMAND_H

#include "props_cmd.h"
#include "props_tracker.h"

namespace edit_cmd {
    const char* const _ALIAS_FILE_    = "alias";
    const char* const _GROUP_SEARCH_  = "group";
    const char* const _MULTI_SEARCH_  = "multi";
    const char* const _IGNORE_CASE_   = "ignore-case";
    const char* const _SEPARATOR_     = "separator";
    const char* const _USE_REGEX_     = "expression";
    const char* const _PARTIAL_MATCH_ = "partial";
    const char *const _EDIT_CMD_      = "set";
}

class PropsEditCommand : public PropsCommand {

public:

    /**
     * Constructor of the Edit command
     */
    PropsEditCommand() {
        id_ = name_  = "set";
        tagLine_     = "Edit values of matched keys in a file or list of tracked files";
        description_ = "Tries to modify the value of a key found using regex/wildcard expression in specific files "
                       "or the list of currently tracked files if no file is supplied."
                       "In case no options are specified, the master file of the tracker is the default file to lookup but "
                       "all tracked files can be queried simultaneously if a global search is performed. It is also possible "
                       "to modify files present in tracker groups, or files using aliases.";

        args_ = { PropsArg::make_arg(edit_cmd::_EDIT_CMD_, { "<key>" ,"<value> [files...]" } , "Searches the files for a given key and replace its current value with the provided one",
                                     { PropsOption::make_opt(edit_cmd::_ALIAS_FILE_, "Replaces the value in a tracked file using the alias", {"<alias>"}),
                                       PropsOption::make_opt(edit_cmd::_USE_REGEX_, "The key is expressed as a regular expression"),
                                       PropsOption::make_opt(edit_cmd::_IGNORE_CASE_, "Performs a case-insensitive search"),
                                       PropsOption::make_opt(edit_cmd::_MULTI_SEARCH_, "Perform a global replacement in all tracked files"),
                                       PropsOption::make_opt(edit_cmd::_PARTIAL_MATCH_, "Allow partial matches"),
                                       PropsOption::make_opt(edit_cmd::_GROUP_SEARCH_, "Perform a modification on files present in a tracker group", {"<group_name>"}),
                                       PropsOption::make_opt(edit_cmd::_SEPARATOR_, "Separator between keys and values", {"<separator>"})
                                     })
                 };
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
     * Perform a modification search using the supplied options.
     *
     * @return the search results
     */
    std::unique_ptr<PropsResult> modify();

    /**
     * Retrieves the list of files to lookup from the given options.
     *
     * @param fileList the list of files
     * @param res the output result in case of errors.
     */
    void retrieveFileList(std::list<PropsFile>& fileList, Result& res);

    /**
     * The property tracker
     */
    PropsTracker* propsTracker_{nullptr};

};

#endif //PROPS_EDIT_COMMAND_H
