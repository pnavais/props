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

#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include "arg_store.h"
#include <getopt.h>
#include "props_arg.h"
#include "result.h"

/**
 * Allows parsing sub-commands automatically for a given command.
 */
class ArgParser {
public:

    /**
     * Find the matching argument and retrieve its options
     *
     * @param argStore the arguments store
     * @param args the arguments accepted by the command
     *
     * @return the result of the operation
     */
    static Result parseArgs(ArgStore &argStore, const std::list<PropsArg> &args);

private:

    /**
     * Reads the command line options and retrieves the rest of non
     * option arguments as a list.
     *
     * @param arg      the argument to process
     * @param argStore the arguments store
     * @param result   the result of the operation
     */
    static void readOptions(const PropsArg &arg, ArgStore &argStore, Result& result);

    /**
     * Creates the short/longs options for the parser.
     *
     * @param arg the command arguments
     * @param optionShortList the options short list
     * @param long_opts the long options
     */
    static void createParserArgs(const PropsArg &arg, std::string &optionShortList, option *&long_opts);

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
    static void mapOptions(const PropsArg &arg, ArgStore &argStore, Result &result, int opt);
};

#endif //ARG_PARSER_H
