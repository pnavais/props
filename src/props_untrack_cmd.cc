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

#include "props_untrack_cmd.h"
#include <memory>
#include <props_file_tracker.h>
#include <exec_exception.h>
#include "rang.hpp"

void PropsUntrackCommand::parse(const int& argc, char* argv[]) {
    if (argc > 1) {
        PropsCommand::parse(argc, argv);
    } else {
        throw ExecutionException("No arguments supplied");
    }

    // Keep the file as a regular option considering it is
    // the first non-arg option
    if (optionStore_.getCmdName() == untrack_cmd::_UNTRACKED_FILE_) {
        optionStore_.addOption(untrack_cmd::_UNTRACKED_FILE_, optionStore_.getArgs().front());
    }
}

/**
 * Executes the untrack command either removing a given file
 * from the list of tracked ones using the file name or
 * the alias.
 *
 * @param result the result message for the command
 */
void PropsUntrackCommand::execute(PropsResult &result) {
    std::ostringstream out;

    rang::setControlMode(rang::control::Force);

    if (optionStore_.getCmdName() == untrack_cmd::_UNTRACKED_FILE_)
    {
        untrackFile(out);
    }

    rang::setControlMode(rang::control::Auto);

    result.setResult_(out.str());
}

/**
 * Tracks the selected filed.
 *
 * @param out the output stream
 *
 * @return the result of the operation
 */
Result PropsUntrackCommand::untrackFile(std::ostringstream& out) {
    auto option_map = optionStore_.getOptions();
    auto fileName = option_map[untrack_cmd::_UNTRACKED_FILE_];
    Result res{res::ERROR};

    // Sets the alias (if any)
    if (option_map.count(untrack_cmd::_ALIAS_FILE_) != 0) {
        std::string alias = fileName;
        PropsFile* propsFile = PropsFileTracker::getDefault().getFileWithAlias(alias);
        if (propsFile != nullptr) {
            fileName = propsFile->getFileName();
        }
        res = PropsFileTracker::removeByAlias(alias);
    } else {
        res = PropsFileTracker::remove(fileName);
    }

    if (res.isValid()) {
        out << std::endl << rang::fg::green << "File \"" << fileName << "\" not tracked anymore" << rang::fg::reset << std::endl;
    } else {
        throw ExecutionException(res);
    }

    return res;
}
