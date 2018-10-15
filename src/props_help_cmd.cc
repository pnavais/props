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

#include "props_help_cmd.h"
#include "props_cmd_factory.h"
#include "string_utils.h"
#include "rang.hpp"


const std::string main_opts[][3] = {
        { "track"        , "<option>"  , "Track files | List tracked files"                } ,
        { "untrack"      , "<option"   , "Untrack files"                                   } ,
        { "search"       , ""          , "Search a key in the target files"                } ,
        { "set"          , ""          , "Set the given key in the target files"           } ,
        { "toggle"       , ""          , "Disable/Enable the key in the target files"      } ,
        { "disable"      , ""          , "Disable the key in the target files"             } ,
        { "enable"       , ""          , "Enable the key in the target files"              } ,
        { "remove"       , ""          , "Remove the key in the target files"              } ,
        { "help"         , "<command>" , "Retrieve detailed options for the given command" } ,
        { "-v|--version" , ""          , "Print current version"                           }
};

/**
 * Parses the supplied arguments for the help command
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 */
void PropsHelpCommand::parse(const int &argc, const char **argv) {
    for (int i=0; i<argc; i++) {
        std::cout << "argv["<<i<<"] = [" << argv[i] << "]" << std::endl;
    }
}

/**
 * Executes the help command displaying the usage for the specific
 * command or all available commands
 * general information if no specific command was supplied.
 *
 * @param result the usage message for the command or all commands if no command
 * supplied
 */
void PropsHelpCommand::execute(PropsResult &result) {
    std::ostringstream out;
    rang::setControlMode(rang::control::Force);

    out << "usage: " << rang::fg::green << PACKAGE_NAME << rang::fg::reset << " [command]" << std::endl;
    out << "\nThese are the current available commands : \n" << std::endl;

    for (auto& command : PropsCommandFactory::getDefault().getCommands()) {
        out << "\t" << StringUtils::padding(command->getName() + " " + StringUtils::toFlatString(command->getArgs(), " "), 20)
                  << ":  " << command->getDescription() << std::endl;
    }

    rang::setControlMode(rang::control::Auto);

    result.setResult_(out.str());
}
