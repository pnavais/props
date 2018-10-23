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

#include "props_cmd.h"
#include "string_utils.h"
#include "rang.hpp"
#include <sstream>

/**
  * Parse the command line arguments to initialize the command.
  *
  * @param argc the number of arguments supplied
  * @param argv the array of arguments
  *
  */
void PropsCommand::parse(const int& argc, const char** argv) noexcept(false) {
    (void)argc;
    (void)argv;
};

/**
 * Display a help message
 * on the given output stream.
 */
void PropsCommand::getHelp(std::ostream& out)
{
    rang::setControlMode(rang::control::Force);

    out << std::endl     << rang::fgB::gray << "NAME" << rang::style::reset << std::endl;
    out << "\t" << name_ << " - " << tagLine_     << std::endl;

    out << std::endl << rang::fgB::gray   << "SYNOPSIS"   << rang::style::reset << std::endl;
    out << "\t" << rang::style::underline << PACKAGE_NAME << rang::style::reset;
    out << " "  << rang::style::underline << name_        << rang::style::reset;

    std::string prefix = " ";

    for (auto& arg : args_) {
        if (!arg.getOptions().empty()) {

        }
        out << prefix << (arg.getOptions().empty() ? "" : "[");
        out << "<" << arg.getName() << ">";
        out << (arg.getOptions().empty() ? "" : " <options>...]");
        prefix = " | ";
    }

    out << std::endl << rang::fgB::gray << "DESCRIPTION" << rang::style::reset << std::endl;
    std::list<std::string> descLines = StringUtils::wraptext(description_, 120);
    for (auto& desc : descLines) {
        out << "\t" << desc << std::endl;
    }

    out << std::endl << rang::fgB::gray << "OPTIONS" << rang::style::reset << std::endl;
    //out << description_ ;


    //std::ostringstream subArgs;
    //std::string prefix;
    //std::string cmdPrefix = "Where : \n";
    //for (auto& arg : args_) {
    //    out     << prefix    << "\t" << (arg.isParam() ? "<" : "[") << arg.getName() << (arg.isParam() ? ">" : "]");
    //    subArgs << cmdPrefix << StringUtils::padding(arg.getName()) << ":" << arg.getDescription() << std::endl;
    //    prefix    = " ";
    //    cmdPrefix = "";
    //}
    //out << std::endl;
    //out << subArgs.str() << std::endl;

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
