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

#include <iostream>
#include <sstream>
#include <string_utils.h>
#include <result.h>
#include "arg_parser.h"

/**
 * Find the matching argument and retrieve its options
 *
 * @param argStore the arguments store
 * @param args the arguments accepted by the command
 *
 * @return the result of the operation
 */
Result ArgParser::parseArgs(ArgStore &argStore, const std::list<PropsArg>& args) {

    Result result{res::VALID};
    const PropsArg* propsCmd = nullptr;

    for (const auto& arg : args) {
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

    // Command has ben found, read options
    if (propsCmd != nullptr) {
        argStore.setCmdName(propsCmd->getName());
        readOptions(*propsCmd, argStore, result);
    }

    return result;
}

/**
 * Reads the command line options and retrieves the rest of non
 * option arguments as a list.
 *
 * @param arg      the argument to process
 * @param argStore the arguments store
 * @param result   the result of the operation
 */
void ArgParser::readOptions(const PropsArg& arg, ArgStore& argStore, Result& result) {

    int opt_index = 2;
    if (!arg.getOptions().empty()) {
        std::string optionShortList;
        auto *long_opts = new option[arg.getOptions().size() + 1];
        createParserArgs(arg, optionShortList, long_opts);

        int opt;

        // Retrieve the options
        while (((opt = getopt_long(argStore.getArgc(), argStore.getArgv(),
                                   optionShortList.c_str(),
                                   long_opts, nullptr)) != -1) && result.isValid()) {
            mapOptions(arg, argStore, result, opt);
        }

        delete[] long_opts;
        opt_index = optind;
    }

    // Map non options
    int index;
    for (index = opt_index; index < argStore.getArgc(); index++) {
        argStore.addArg(std::string(argStore.getArgv()[index]));
    }

}

/**
 * Parses an argument option trying to map against the correct command
 * options.
 *
 * @param arg the command arguments
 * @param argStore the arguments store
 * @param result the validation result
 * @param opt the parsed option
 * @return the validation result
 */
void ArgParser::mapOptions(const PropsArg &arg, ArgStore &argStore, Result &result, int opt) {

    // Map errors
    if (opt == ':') {
        result = res::ERROR;
        result.setMessage(std::string("Option \"") + char(optopt) + "\" requires an argument");
    } else if ((optarg != nullptr) && ((optarg[0] == '-') || StringUtils::isWhiteSpace(optarg))) {
        result = res::ERROR;
        result.setMessage(std::string("Invalid argument supplied \"") + optarg + "\" for option \"" + char(opt) + "\"");
    }
    else if (opt == '?') {
        result = res::ERROR;
        auto errorOption = (optopt != 0) ? std::string("\"") + char(optopt) + "\"" : "";
        result.setMessage(std::string("Invalid option supplied ") + errorOption);
    } else {
        for (const auto &opt_arg : arg.getOptions()) {
            if (opt_arg.getShortName() == opt) {
                if (optarg != nullptr) {
                    argStore.addOption(opt_arg.getName(), optarg);
                } else {
                    argStore.addOption(opt_arg.getName(), "true");
                }
            }
        }
    }
}

/**
 * Creates the short/longs options for the parser.
 *
 * @param arg the command arguments
 * @param optionShortList the options short list
 * @param long_opts the long options
 */
void ArgParser::createParserArgs(const PropsArg &arg, std::string& optionShortList, option *&long_opts) {

    int optNum = 0;
    std::ostringstream opShortStream;

    // Hide getopt errors
    opShortStream << ":";

    // Create the short and long options
    for (const auto &opt : arg.getOptions()) {
        opShortStream << opt.getShortName() << (!opt.getCmdList().empty() ? ":" : "");
        long_opts[optNum++] = {opt.getName().c_str(), (!opt.getCmdList().empty() ? required_argument : no_argument),
                               nullptr, opt.getShortName()};
    }

    optionShortList = opShortStream.str();
    long_opts[optNum] = {nullptr, no_argument, nullptr, 0};
}
