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
#include <memory>

/**
 * Parses the supplied arguments for the help command
 *
 * @param argc the number of arguments
 * @param argv the list of arguments
 */
void PropsTrackCommand::parse(const int& argc, char* argv[]) noexcept(false) {
    std::cout << "Parsing " << argc << " arguments" << std::endl;

    if (argc > 1) {
        ArgStore argOutput = ArgStore::make_store(argc, argv);
        parseArgs(argOutput);

        for (const auto& arg : argOutput.getArgs()) {
            std::cout << "Non-Arg : [" << arg << "]" << std::endl;
        }

        for (const auto& opt : options_) {
            std::cout << opt.first << " -> [" << opt.second << "]" << std::endl;
        }
    }

}

/**
 * Find the matching argument and retrieve its options
 *
 * @param argStore the arguments store
 */
void PropsTrackCommand::parseArgs(ArgStore& argStore) {

    const PropsArg* propsCmd = nullptr;

    for (const auto& arg : args_) {
        std::cout << "ARG >> " << arg.getName() << "|" << (arg.isAnonymous() ? "*" : "") << std::endl;
        const std::string& argName = arg.getName();

        // Check that the arg is present as first argument
        if (!arg.isAnonymous()) {
            if (argStore.getArgv()[1] == argName) {
                propsCmd = &arg;
                break;
            }
        } else if (propsCmd == nullptr) {
            propsCmd = &arg;
        }
    }

    if (propsCmd != nullptr) {
        std::cout << "ARG FOUND [" << propsCmd->getName() << "]" << std::endl;
        if (!readOptions(*propsCmd, argStore)) {
            exit(4);
        }
    }

}

/**
 * Reads the command line options and retrieves the rest of non
 * option arguments as a list.
 *
 * @param arg      the argument to process
 * @param argStore the arguments store
 */
bool PropsTrackCommand::readOptions(const PropsArg& arg, ArgStore& argStore) {

    bool isValid = true;

    if (!arg.getOptions().empty()) {

        // Create the short/long options
        int optNum = 0;
        std::ostringstream optionShortList;
        auto *long_opts = new option[arg.getOptions().size() + 1];

        optionShortList << ":"; // Hide getopt errors
        for (const auto &opt : arg.getOptions()) {
            optionShortList << opt.getShortName() << (!opt.getCmdList().empty() ? ":" : "");
            long_opts[optNum++] = {opt.getName().c_str(), (!opt.getCmdList().empty() ? required_argument : no_argument),
                                   nullptr, opt.getShortName()};
        }

        long_opts[optNum] = {nullptr, no_argument, nullptr, 0};

        int opt;
        // Retrieve the options
        while (((opt = getopt_long(argStore.getArgc(), argStore.getArgv(),
                optionShortList.str().c_str(),
                long_opts, nullptr)) != -1) && isValid) {

            // Map errors
            if (opt == ':') {
                std::cerr << rang::fgB::red << "Option \"" << char(optopt)  << "\" requires an argument" << rang::fg::reset << std::endl;
                isValid = false;
            } else if ((optarg != nullptr) && (optarg[0] == '-')) {
                std::cerr << rang::fgB::red << "Option \"" << char(optopt) << "\" requires an argument" << rang::fg::reset << std::endl;
                isValid = false;
            }
            else if (opt == '?') {
                std::cout << " => " << argStore.getArgv()[optind] << "]" << std::endl;
                std::cerr << rang::fgB::red << "Invalid option supplied \"" << char(optopt)  << "\"" << rang::fg::reset << std::endl;
                isValid = false;
            } else {
                for (const auto &opt_arg : arg.getOptions()) {
                    if (opt_arg.getShortName() == opt) {
                        if (optarg != nullptr) {
                            options_[opt_arg.getName()] = optarg;
                        } else {
                            options_[opt_arg.getName()] = "true";
                        }
                    }
                }
            }
        }

        delete[] long_opts;

        // Map non options
        int index;
        for (index = optind; index < argStore.getArgc(); index++) {
            argStore.addArg(std::string(argStore.getArgv()[index]));
        }
    }

    return isValid;

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
