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
#include "props_tracker_factory.h"
#include "exec_exception.h"

void PropsTrackerCommand::parse(const int& argc, char* argv[]) {
    if (argc > 1) {
        propsTracker_ = &PropsTrackerFactory::getDefaultTracker();
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
    Result res{res::VALID};

    if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_ADD_CMD_) {
        res = trackFile();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_LS_CMD_) {
        propsTracker_->listTracked();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNALIAS_CMD_) {
        res = unalias();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNTRACK_CMD_) {
        res = untrackFile();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_MASTER_CMD_) {
        res = setMaster();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_SET_ALIAS_CMD_) {
        res = setAlias();
    }

    res.showMessage(out);
    result.setResult(res);
    result.setOutput(out.str());
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
    // Sets group (if available)
    if (option_map.count(tracker_cmd::_GROUP_NAME_) != 0) {
        propsFile.setGroup(option_map.at(tracker_cmd::_GROUP_NAME_));
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

    // Use name as alias
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        propsFile = propsTracker_->getFileWithAlias(*inputFile);
        hasAlias = true;
    } else {
        propsFile = propsTracker_->getFile(*inputFile);
        if (propsFile == nullptr) {
            // Maybe relative ?
            const std::string& absolutePath = FileUtils::getAbsolutePath(*inputFile);
            if (FileUtils::fileExists(absolutePath)) {
                propsFile = propsTracker_->getFile(absolutePath);
                if (propsFile == nullptr) {
                    // Track it automatically
                    PropsFile newFile = PropsFile::make_file(absolutePath);
                    res = propsTracker_->add(newFile);
                    res.showMessage();
                    propsFile = propsTracker_->getFile(absolutePath);
                }
            }
        }
    }

    if (res.isValid() && (propsFile != nullptr)) {
        if (propsTracker_->getMasterFile() != propsFile) {
            propsTracker_->updateMasterFile(propsFile);
            res = propsTracker_->save();
            if (res.isValid()) {
                res.setMessage("File \"" + propsFile->getFileName() + "\" set as new master");
            }
        } else {
            res = res::ERROR;
            res.setSeverity(res::WARN);
            res.setMessage("File \"" + propsFile->getFileName() + "\" is already master");
        }
    } else {
        res = res::ERROR;
        res.setMessage(std::string((hasAlias ? "Alias" : "File")) + " \"" + *inputFile + "\" not found");
    }

    return res;
}

/**
 * Sets an alias for a given file.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::setAlias() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();
    // Check alias is set
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        const std::string& file  = option_map.at(tracker_cmd::_TRACKED_FILE_);
        const std::string& alias = option_map.at(tracker_cmd::_ALIAS_FILE_);

        // Lookup File
        PropsFile* propsFile = propsTracker_->getFile(file);
        if (propsFile == nullptr) {
            // Maybe relative ?
            const std::string &absolutePath = FileUtils::getAbsolutePath(file);
            if (FileUtils::fileExists(absolutePath)) {
                propsFile = propsTracker_->getFile(absolutePath);
            }
        }

        if (propsFile != nullptr) {
            res = propsTracker_->setAlias(file, alias);
        } else {
            res = res::ERROR;
            res.setMessage("File \"" + file +"\" not found");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Alias not specified");
    }

    return res;
}

/**
 * Removes an alias from a given file.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::unalias() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();

    // Use name as alias
    if (option_map.count(tracker_cmd::_ALIAS_FILE_) != 0) {
        res = propsTracker_->removeAlias(option_map.at(tracker_cmd::_TRACKED_FILE_));
    } else {
        res = propsTracker_->removeFileAlias(option_map.at(tracker_cmd::_TRACKED_FILE_));
    }

    return res;
}
