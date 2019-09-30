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

#include "props_track_cmd.h"
#include <memory>
#include <props_file_tracker.h>
#include <exec_exception.h>
#include "rang.hpp"

void PropsTrackCommand::parse(const int& argc, char* argv[]) {
    if (argc > 1) {
        PropsCommand::parse(argc, argv);
    } else {
        throw ExecutionException("No arguments supplied");
    }

    // Keep the file as a regular option considering it is
    // the first non-arg option
    if ((optionStore_.getCmdName() == track_cmd::_ALIAS_FILE_) || 
        (optionStore_.getCmdName() == track_cmd::_TRACKER_UNALIAS_CMD_)) {
        optionStore_.addOption(track_cmd::_TRACKED_FILE_, optionStore_.getArgs().front());
    }
}

/**
 * Executes the track command either adding a given file
 * to the list of tracked ones or displaying the information
 * of this list.
 *
 * @param result the result message for the command
 */
void PropsTrackCommand::execute(PropsResult &result) {
    std::ostringstream out;

    rang::setControlMode(rang::control::Force);

    if (optionStore_.getCmdName() == track_cmd::_TRACKER_ADD_CMD_)
    {
        trackFile(out);
    } else if (optionStore_.getCmdName() == track_cmd::_TRACKER_LS_CMD_) {
        PropsFileTracker::getDefault().listTracked();
    } else if (optionStore_.getCmdName() == track_cmd::_TRACKER_UNALIAS_CMD_) {
        auto &alias = optionStore_.getArgs().front();
        std::cout << "Hay que borrar el alias => [" << alias << "]" << std::endl;
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
Result PropsTrackCommand::trackFile(std::ostringstream& out) {
    auto option_map = optionStore_.getOptions();
    PropsFile propsFile = PropsFile::make_file(option_map[track_cmd::_TRACKED_FILE_]);

    // Sets the alias (if any)
    if (option_map.count(track_cmd::_ALIAS_FILE_) != 0) {
        propsFile.setAlias(option_map[track_cmd::_ALIAS_FILE_]);
    }
    // Sets as master (if available)
    if (option_map.count(track_cmd::_MASTER_FILE_) != 0) {
        propsFile.setMaster(true);
    }

    // Adds the file to the tracker
    Result res = PropsFileTracker::add(propsFile);
    if (res.isValid()) {
        out << std::endl << rang::fg::green << "Now tracking \"" << propsFile.getFileName() << "\"" << rang::fg::reset << std::endl;
    } else {
        throw ExecutionException(res);
    }

    return res;
}
