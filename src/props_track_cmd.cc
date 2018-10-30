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
#include <getopt.h>

/**
 * Parses the supplied arguments for the help command
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 */
void PropsTrackCommand::parse(const int& argc, char* argv[]) noexcept(false) {
    std::cout << "Parsing " << argc << " arguments" << std::endl;
    std::string mandanga;

    if (argc > 1) {

        const char* const short_opts = "m:";
        const option long_opts[] = {  {"master", required_argument, nullptr, 'm' } ,
                                      { nullptr, no_argument, nullptr,   0 } };

        while (true) {
            const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);

            if (-1 == opt) {
                std::cout << "FIN DE CICLO [" << opt << "]" << std::endl;
                break;
            }

            switch (opt) {
                case 'm':
                    options_[track_cmd::_TRACKED_FILE_] = std::string(optarg);
                    break;

                case '?':
                default:
                    break;
            }

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
 */
void PropsTrackCommand::execute(PropsResult &result) {
    std::ostringstream out;
    rang::setControlMode(rang::control::Force);

    if (options_.count(track_cmd::_TRACKED_FILE_) > 0)
    {
        out << rang::fg::green << "File \"" << options_[track_cmd::_TRACKED_FILE_] << "\" added to tracked list" << rang::fg::reset << std::endl;
    }

    rang::setControlMode(rang::control::Auto);

    result.setResult_(out.str());
}
