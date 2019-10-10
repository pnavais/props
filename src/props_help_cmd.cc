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

#include "props_help_cmd.h"
#include "props_cmd_factory.h"
#include "string_utils.h"
#include "rang.hpp"

/**
 * Parses the supplied arguments for the help command
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 */
void PropsHelpCommand::parse(const int &argc, char *argv[]) {
    if (argc > 1) {
        subCmd_ = argv[1];

        PropsCommand* pPtr = PropsCommandFactory::getDefault().getCommand(StringUtils::toUpper(subCmd_));
        if (pPtr != nullptr) {
            std::ostringstream out;
            pPtr->getHelp(out);
            helpMessage_ = out.str();
        }
    }
}

/**
 * Executes the help command displaying the usage for the specific
 * command or all available commands
 * general information if no specific command was supplied.
 *
 * @param result the usage message for the command or all commands if no command
 * supplied
 * @retun the result of the command
 */
std::unique_ptr<PropsResult> PropsHelpCommand::execute() {
    auto result = new PropsResult();
    std::ostringstream out;
    rang::setControlMode(rang::control::Force);

    if (!helpMessage_.empty()) {
        out << helpMessage_;
    } else {
        if (!subCmd_.empty()) {
            out << rang::fg::red << "Command \"" << subCmd_ << "\" not available" << rang::fg::reset << std::endl;
        } else {
            out << rang::fg::yellow << "No command specified" << rang::fg::reset << std::endl;
        }
        out << "\nThese are the current available commands : \n" << std::endl;

        for (auto& command : PropsCommandFactory::getDefault().getCommands()) {
            std::string summary = (command->getSummaryArg().empty()) ? "" : "<" + command->getSummaryArg() + ">";
            out << "\t" << StringUtils::padding(command->getName() + " " + summary, 15)
                << ":  " << command->getTagline() << std::endl;
        }
    }

    rang::setControlMode(rang::control::Auto);

    result->setOutput(out.str());

    return std::unique_ptr<PropsResult>(result);
}
