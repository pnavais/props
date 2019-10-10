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

#include <file_utils.h>
#include "props_tracker_cmd.h"
#include "props_tracker_factory.h"
#include "exec_exception.h"
#include <sstream>

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
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNGROUP_CMD_) {
        optionStore_.addOption(tracker_cmd::_GROUP_NAME_, optionStore_.getArgs().front());
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_RENAME_GROUP_CMD_) {
        optionStore_.addOption(tracker_cmd::_OLD_GROUP_NAME_, optionStore_.getArgs().front());
        optionStore_.addOption(tracker_cmd::_GROUP_NAME_, *std::next(optionStore_.getArgs().begin(), 1));
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_GROUP_CMD_) {
        optionStore_.addOption(tracker_cmd::_TRACKED_FILE_, optionStore_.getArgs().front());
        optionStore_.addOption(tracker_cmd::_GROUP_NAME_, *std::next(optionStore_.getArgs().begin(), 1));
    }

}

/**
 * Executes the track command either adding a given file
 * to the list of tracked ones or displaying the information
 * of this list.
 *
 * @param result the result message for the command
 */
std::unique_ptr<PropsResult> PropsTrackerCommand::execute() {
    auto result = new PropsResult;
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
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_GROUP_CMD_) {
        res = group();
    }  else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_UNGROUP_CMD_) {
        res = ungroup();
    }  else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_RENAME_GROUP_CMD_) {
        res = renameGroup();
    } else if (optionStore_.getCmdName() == tracker_cmd::_TRACKER_CLEAR_CMD_) {
        res = propsTracker_->clear();
    }

    res.showMessage(out);
    result->setResult(res);
    result->setOutput(out.str());

    return std::unique_ptr<PropsResult>(result);
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

/**
 * Moves a file to a group from the tracker.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::group() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();
    const std::string* sourceObject = nullptr;
    const std::string* targetGroup = nullptr;

    // Obtain parameters
    if (option_map.count(tracker_cmd::_TRACKED_FILE_) != 0) {
        sourceObject = &option_map.at(tracker_cmd::_TRACKED_FILE_);
        if (option_map.count(tracker_cmd::_GROUP_NAME_) != 0) {
            targetGroup = &option_map.at(tracker_cmd::_GROUP_NAME_);
            res = propsTracker_->group(*sourceObject, *targetGroup, option_map.count(tracker_cmd::_ALIAS_FILE_) != 0);
        } else {
            res = res::ERROR;
            res.setMessage("Missing target group");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Missing source file/alias");
    }

    return res;
}

/**
 * Removes an a group from the tracker, effectively
 * moving all its contained files to the default
 * group.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::ungroup() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();

    if (option_map.count(tracker_cmd::_GROUP_NAME_) != 0) {
        res = propsTracker_->removeGroup(option_map.at(tracker_cmd::_GROUP_NAME_));
    } else {
        res = res::ERROR;
        res.setMessage("No group specified");
    }

    return res;
}

/**
 * Renames an existing group from the tracker.
 * effectively moving all its contained files to the target
 * group.
 *
 * @return the result of the operation
 */
Result PropsTrackerCommand::renameGroup() {
    Result res{res::VALID};
    const auto& option_map = optionStore_.getOptions();
    const std::string* sourceGroup = nullptr;
    const std::string* targetGroup = nullptr;

    // Obtain parameters
    if (option_map.count(tracker_cmd::_OLD_GROUP_NAME_) != 0) {
        sourceGroup = &option_map.at(tracker_cmd::_OLD_GROUP_NAME_);
        if (option_map.count(tracker_cmd::_OLD_GROUP_NAME_) != 0) {
            targetGroup = &option_map.at(tracker_cmd::_GROUP_NAME_);
            res = propsTracker_->renameGroup(*sourceGroup, *targetGroup, option_map.count(tracker_cmd::_FORCE_MOVE_) != 0);
        } else {
            res = res::ERROR;
            res.setMessage("Missing target group");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Missing source group");
    }

    return res;
}