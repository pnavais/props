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


#include "props_cli.h"
#include "props_cmd_factory.h"
#include <cstring>
#include <string_utils.h>
#include <props_config.h>

/**
 * Parses the command line arguments to
 * provide the props command instance.
 *
 * @param argc the number of arguments
 * @param argv the array of arguments
 * @return the props parameter holder
 */
PropsCommand* PropsCLI::parse(const int& argc, char* argv[])
{
    PropsCommand* command = nullptr;

	// Check minimum options
	if (argc < 2) {
        command = PropsCommandFactory::getDefault().getCommand(p_cli::DEFAULT_HELP_CMD_ID);
	} else {
	    if (strcmp(argv[1], "help")==0) {
            command = PropsCommandFactory::getDefault().getCommand(p_cli::DEFAULT_HELP_CMD_ID);
            command->parse(argc-1, argv+1);
	    }
	    else if ((strcmp(argv[1], "--version")==0) || (strcmp(argv[1], "-v")==0)) {
            command = PropsCommandFactory::getDefault().getCommand(p_cli::DEFAULT_VERSION_CMD_ID);
	    } else {
            command = PropsCommandFactory::getDefault().getCommand(StringUtils::toUpper(argv[1]));
            if (command != nullptr) {
                command->parse(argc-1, argv+1);
            } else {
                // Check alias as last resort
                char **argv_ext;
                int argc_ext = PropsCLI::getAliasArgs(argc, argv, argv_ext);
                command = PropsCommandFactory::getDefault().getCommand(StringUtils::toUpper(argv_ext[1]));
                if (command != nullptr) {
                    command->parse(argc_ext-1, argv_ext+1);
                } else {
                    command = PropsCommandFactory::getDefault().getUnknownCommand(argv[1]);
                }
            }
	    }
	}

	return (command != nullptr) ? command : nullptr;
}

/**
 * Parses the command line arguments to
 * provide the props command instance.
 *
 * @param argc the number of arguments
 * @param argv the array of arguments
 * @param argv_ext the new array of arguments for the alias or null
 * if alias not found
 */
int PropsCLI::getAliasArgs(const int& argc, char* argv[], char**& argv_ext) {

    int argc_ext = -1;
    const std::string* alias = PropsConfig::getDefault().getValue(std::string("alias.") + argv[1]);

    if (alias != nullptr) {
        char **argv_alias;

        // Split alias
        int ret = StringUtils::split_cmdline(*alias, argv_alias);

        // Replace argv with alias args
        if (ret >= 0) {
            argc_ext = ret + (argc - 1);
            argv_ext = new char *[argc_ext];
            argv_ext[0] = new char[sizeof(argv[0])];
            strcpy(argv_ext[0], argv[0]);

            for (int i = 0; i < ret; i++) {
                argv_ext[i + 1] = new char[sizeof(argv_alias[i])];
                strcpy(argv_ext[i + 1], argv_alias[i]);
                delete[] argv_alias[i];
            }
            delete[] argv_alias;

            for (int i = 2; i < argc; i++) {
                argv_ext[ret + i - 1] = new char[sizeof(argv[i])];
                strcpy(argv_ext[ret + i - 1], argv[i]);
            }
        } else {
            throw InitializationException("Error parsing alias [" + *alias + "] : " + str_errors::get_error(ret));
        }
    }

    return argc_ext;
}
