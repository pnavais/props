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

#include "props_track_cmd.h"
#include "string_utils.h"
#include "rang.hpp"


/**
 * Parses the supplied arguments for the help command
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 */
void PropsTrackCommand::parse(const int& argc, const char** argv) noexcept(false) {
    if (argc > 1) {
        //subCmd_ = argv[1];

        //std::unique_ptr<PropsCommand> *pPtr = PropsCommandFactory::getDefault().getCommand(StringUtils::toUpper(subCmd_));
        //if (pPtr != nullptr) {
        //    std::ostringstream out;
        //    pPtr->get()->getHelp(out);
        //    helpMessage_ = out.str();
        //}
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
void PropsTrackCommand::execute(PropsResult &result) {
    std::ostringstream out;
    rang::setControlMode(rang::control::Force);

    out << rang::fg::green << "File \"" << options_[track_cmd::_TRACKED_FILE_] << "\" added to tracked list" << rang::fg::reset << std::endl;

    rang::setControlMode(rang::control::Auto);

    result.setResult_(out.str());
}