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
#include "string_utils.h"
#include "props_cmd_factory.h"
#include "props_version_cmd.h"
#include "props_help_cmd.h"
#include "props_unknown_cmd.h"
#include "props_tracker_cmd.h"

/**
 * Adds all available commands
 */
PropsCommandFactory::PropsCommandFactory() {

    listCmds_.push_back(std::unique_ptr<PropsCommand>(new PropsTrackerCommand()));
    listCmds_.push_back(std::unique_ptr<PropsCommand>(new PropsHelpCommand()));
    listCmds_.push_back(std::unique_ptr<PropsCommand>(new PropsVersionCommand()));

    //TODO: Load commands dynamically
    for (auto& cmd : listCmds_) {
        commandMap_[StringUtils::toUpper(cmd->getId())] = cmd.get();
    }
}

/**
 * Retrieves the reference to the command identified by the given id
 * or null if not found.
 *
 * @param id the identifier
 * @return
 */
PropsCommand* PropsCommandFactory::getCommand(const std::string &id) const {
    PropsCommand* command = nullptr;
    if (commandMap_.find(id) != commandMap_.end()) {
        command = commandMap_.at(id);
    }

    return command;
}

/**
 * Creates a wrapper command for non a recognized command
 * argument.
 *
 * @param command the command not recognized
 * @return the wrapped command
 */
PropsCommand* PropsCommandFactory::getUnknownCommand(const std::string& command) {
    std::string commandName = "UNKNOWN_CMD_"+command;
    if (commandMap_.count(commandName) == 0) {
        auto unknownCmd = std::unique_ptr<PropsCommand>(new PropsUnknownCommand(command));
        commandMap_[commandName] = unknownCmd.get();
        listCmds_.push_back(std::move(unknownCmd));
    }
    return commandMap_[commandName];
}
