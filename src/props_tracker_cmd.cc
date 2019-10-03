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

#include <memory>
#include <file_utils.h>
#include "props_tracker_cmd.h"
#include "props_file_tracker.h"
#include "exec_exception.h"

void PropsTrackerCommand::parse(const int& argc, char* argv[]) {
    if (argc > 1) {
        PropsCommand::parse(argc, argv);
    } else {
        throw ExecutionException("No arguments supplied");
    }

    // Keep the file as a regular option considering it is
    // the first non-arg option
    if ((optionStore_.getCmdName() == tracker_cmd::_TRACKER_ADD_CMD_) ||
        (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_MASTER_CMD_) ||
        (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_ALIAS_CMD_)  ||
        (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNALIAS_CMD_)    ||
        (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNTRACK_CMD_)) {
        optionStore_.addOption(tracker_cmd::_TRACKED_FILE_, optionStore_.getArgs().front());
    }
}

/**
 * Executes the track command either adding a given file
 * to the list of tracked ones or displaying the information
 * of this list.
 *
 * @param result the result message for the command
 */
void PropsTrackerCommand::execute(PropsResult &result) {
    std::ostringstream out;
    const auto& option_map = optionStore_.getOptions();
    Result res{res::VALID};

    if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_ADD_CMD_) {
        res = trackFile();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_LS_CMD_) {
        propsTracker_->listTracked();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNALIAS_CMD_) {
        res = propsTracker_->removeAlias(option_map.at(tracker_cmd::_TRACKED_FILE_));
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNTRACK_CMD_) {
        res = untrackFile();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_MASTER_CMD_) {
        res = setMaster();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_ALIAS_CMD_) {
        // Check alias is set
        if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
            std::cout << "Setting alias [" << option_map.at(tracker_cmd::_TRACKED_FILE_) << "]" << std::endl;
            std::cout << "ES UN ALIAS !" << option_map.at(tracker_cmd::_ALIAS_FILE_) << "]" << std::endl;
        } else {
            res = res::ERROR;
            res.setMessage("Alias not specified");
        }
    }

    res.showMessage(out);
    result.setResult_(out.str());
}

/**
 * Tracks the selected filed.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::trackFile() {
    const auto& option_map = optionStore_.getOptions();
    PropsFile propsFile = PropsFile::make_file(option_map.at(tracker_cmd::_TRACKED_FILE_));

    // Sets the alias (if any)
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        propsFile.setAlias(option_map.at(tracker_cmd::_ALIAS_FILE_));
    }
    // Sets as master (if available)
    if (option_map.count(tracker_cmd::_MASTER_FILE_) != 0) {
        propsFile.setMaster(true);
    }

    // Adds the file to the tracker
    Result res = propsTracker_->add(propsFile);
    if (res.isValid()) {
        res.setMessage("Now tracking \"" + propsFile.getFileName() + "\"");
    }

    return res;
}

/**
 * Removes from the tracker the selected filed using the file
 * name or the alias (if set).
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::untrackFile() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();

    // Use name as alias
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        res = propsTracker_->removeByAlias(option_map.at(tracker_cmd::_TRACKED_FILE_));
    } else {
        res = propsTracker_->remove(option_map.at(tracker_cmd::_TRACKED_FILE_));
    }

    return res;
}


/**
 * Sets the given file/alias as the new master.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::setMaster() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();
    PropsFile* propsFile = nullptr;
    bool hasAlias = false;
    const std::string* inputFile = &(option_map.at(tracker_cmd::_TRACKED_FILE_));

    // Use name as hasAlias
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        propsFile = propsTracker_->getFileWithAlias(*inputFile);
        hasAlias = true;
    } else {
        propsFile = propsTracker_->getFile(*inputFile);
        if (propsFile == nullptr) {
            // Maybe relative ?
            const std::string &absolutePath = FileUtils::getAbsolutePath(*inputFile);
            if (FileUtils::fileExists(absolutePath)) {
                propsFile = propsTracker_->getFile(absolutePath);
                if (propsFile == nullptr) {
                    std::cout << "Oye, que esto no estaba |" << absolutePath << "|" << std::endl;
                    // Track it automatically
                    PropsFile newFile = PropsFile::make_file(absolutePath);
                    res = propsTracker_->add(newFile);
                    res.showMessage();
                    propsFile = &newFile;
                }
            }
        }
    }

    if (res.isValid() && (propsFile != nullptr)) {
        propsTracker_->updateMasterFile(propsFile);
        res.setMessage("File \"" + propsFile->getFileName() + "\" set as new master");
    } else {
        res = res::ERROR;
        res.setMessage(std::string((hasAlias ? "Alias" : "File")) + " \"" + *inputFile + "\" not found");
    }

    return res;
}
