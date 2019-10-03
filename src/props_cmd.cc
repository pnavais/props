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

#include "props_cmd.h"
#include "string_utils.h"
#include "rang.hpp"
#include <config.h>
#include <sstream>
#include <arg_store.h>
#include <arg_parser.h>
#include <exec_exception.h>

/**
  * Parse the command line arguments to initialize the command.
  *
  * @param argc the number of arguments supplied
  * @param argv the array of arguments
  *
  */
void PropsCommand::parse(const int& argc, char* argv[]) noexcept(false) {

    if (argc > 1) {
		if ((argc == 2) && (strcmp(argv[1],"--help")==0)) {
			getHelp(std::cout);
		} else {
			optionStore_ = ArgStore::make_store(argc, argv);
			Result result = ArgParser::parseArgs(optionStore_, args_);
			if (!result.isValid()) {
				throw InitializationException(result.getMessage());
			}
		}
    }

}

/**
 * Display a help message
 * on the given output stream.
 */
void PropsCommand::getHelp(std::ostream& out)
{
    rang::setControlMode(rang::control::Force);

    out << std::endl     << rang::fgB::gray << "NAME" << rang::fg::reset << std::endl;
    out << "\t" << name_ << " - " << tagLine_     << std::endl;

    out << std::endl << rang::fgB::gray   << "SYNOPSIS"   << rang::fg::reset << std::endl;
    out << "\t" << rang::style::underline << PACKAGE_NAME << rang::style::reset;
    out << " "  << rang::style::underline << name_        << rang::style::reset;

    std::string prefix = " ";
    std::ostringstream options;
    std::ostringstream desc_cmd;

    // Find max sizes
    size_t max_size = 0;
    for (auto& arg : args_) {
        const auto& current_size = arg.getName().size();
        max_size = (current_size > max_size) ? current_size : max_size;
    }

    // Display argument information
    for (auto& arg : args_) {
        out << prefix << (arg.getOptions().empty() ? "" : "[");
        out << arg.getName();
        for (auto& attachedArg : arg.getAttachedArgs()) {
            out << " " << attachedArg;
        }
        out << (arg.getOptions().empty() ? "" : " <options>...]");
        prefix = " | ";

        desc_cmd << "\t\t" << StringUtils::padding("<"+arg.getName()+">", max_size+2)
                            << ":  " << arg.getDescription() << std::endl;

        if (!arg.getOptions().empty()) {
            options << rang::fgB::yellow << "\n " << arg.getName();
            for (auto& attachedArg : arg.getAttachedArgs()) {
                options << " " << attachedArg;
            }
            options << " :" << rang::fg::reset << std::endl;
        }

        for (auto& o : arg.getOptions()) {
            options << "\t" << "-" << o.getShortName() << ",--"<< o.getName() << " " << StringUtils::toFlatString(o.getCmdList(), " ") << std::endl
                    << "\t\t" << o.getDescription() << std::endl;
        }
    }

    out << std::endl << std::endl << rang::fgB::gray << "DESCRIPTION" << rang::style::reset << std::endl;
    std::list<std::string> descLines = StringUtils::fitText(description_, 50);
    for (auto& desc : descLines) {
        out << "\t" << desc << std::endl;
    }

    out << "\n\tThis is the description of the arguments supported by the command : \n" << std::endl;
    out << desc_cmd.str() << std::endl;

    const auto& optStr = options.str();
    if (!optStr.empty()) {
        out << std::endl << rang::fgB::gray << "OPTIONS" << rang::style::reset << std::endl;
        out << optStr << std::endl;
    }

    rang::setControlMode(rang::control::Auto);
}

/**
 * Executes the command and formats its results
 * using the standard output.
 */
void PropsCommand::run() noexcept(false)
{
    PropsResult result;
    execute(result);
    result.format(std::cout);
}
