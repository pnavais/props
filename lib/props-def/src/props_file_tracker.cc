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

#include "props_file_tracker.h"
#include "file_utils.h"
#include <exec_exception.h>
#include <init_exception.h>
#include <string_utils.h>
#include <parser/toml.hpp>
#include <props_config.h>


/**
 * Prototypes for local functions
 */
std::string normalizeGroup(const std::string& group);

/**
 * Namespace for constants
 */
namespace tracker {
    static const char* TRACKER_CONFIG_FILE_NAME = "props-tracker.conf";
    static const long DEFAULT_MAX_TRACKED_FILES = 20;
    static const char* MAX_TRACKED_FILES = "general.max_tracked_files";
}

/**
 * Normalizes the group in case the default one
 * is used.
 *
 * @param group the group
 * @return the normalized group name
 */
std::string normalizeGroup(const std::string& group) {
    std::string nGroup = group;
    if (group == std::string(tracker::DEFAULT_GROUP).erase(0,1)) {
        nGroup = tracker::DEFAULT_GROUP;
    } else if (group.empty()) {
        nGroup = tracker::DEFAULT_GROUP;
    }

    return nGroup;
}

/**
 * Default constructor. Reads tracker config
 * if available.
 */
PropsFileTracker::PropsFileTracker() {
    maxTrackedFiles_ = PropsConfig::getDefault().getValue<long>(tracker::MAX_TRACKED_FILES, tracker::DEFAULT_MAX_TRACKED_FILES);
    masterFile_ = nullptr;
    parseTrackerConfig();
}

/**
 * Parses the tracker current configuration from the default
 * user's config file.
 */
void PropsFileTracker::parseTrackerConfig() {
    auto configFilePath = config::CONFIG_FULL_PATH() + tracker::TRACKER_CONFIG_FILE_NAME;
    std::string masterFileName;
    Result result{res::VALID};

    if (FileUtils::fileExists(configFilePath)) {
        try {
            const auto data = toml::parse(configFilePath);

            toml::value trackingSection = toml::find<toml::value>(data, "Tracking");
            toml::array fileArray = toml::find<toml::array>(trackingSection, "files");
            for (auto &file : fileArray) {
                toml::value fileTable = toml::get<toml::value>(file);

                const auto alias     = toml::find_or<std::string>(fileTable, "alias", "");
                const auto master    = toml::find_or<bool>(fileTable, "master", false);
                const auto group     = toml::find_or<std::string>(fileTable, "group", "");
                const auto &location = toml::find<std::string>(fileTable, "location");

                // Creates the new props file
                PropsFile propsFile;
                propsFile.setFileName(location);
                propsFile.setAlias(alias);
                propsFile.setMaster(master);
                propsFile.setGroup(group);
                result = storeFile(propsFile);
                if (!result.isValid()) {
                    std::string msg = "WARN: "+result.getMessage()+".Skipping";
                    result.setMessage(msg);
                }
                result.showMessage();
            }
        } catch (std::exception &e) {
            throw InitializationException("Error parsing tracker configuration file. Details : " + std::string(e.what()));
        }

        // Sets first tracked file as master if not set yet
        if (masterFile_ == nullptr) {
            setFirstAsMaster();
        }
    }
}

/**
 * Sets the front of the tracked files list (if any)
 * as master.
 */
void PropsFileTracker::setFirstAsMaster() {
    if (!trackedFiles_.empty()) {
        auto &front = trackedFiles_.front();
        front.setMaster(true);
        updateMasterFile(&front);
    }
}

/**
 * Adds the given files to the tracker
 *
 * @param files the file to add
 * @param result the result of the operation
 */
void PropsFileTracker::addFiles(std::list<PropsFile> &files, Result &result) {

    std::ostringstream errorStream;
    size_t numFailed = 0;
    res::severity severity{res::NORMAL};
    char prefix = '\0';
    for (auto& file : files) {
        Result partialResult{res::VALID};
        addFile(file, partialResult);
        if (!result.isValid()) {
            errorStream << prefix << result.getMessage();
            prefix = '\n';
            numFailed++;
            severity = (severity != res::CRITICAL) ? result.getSeverity() : severity;
        }
    }

    size_t numCorrect = files.size() - numFailed;
    std::ostringstream outputMsg;


    if (numFailed > 0) {
        result = res::ERROR;
        result.setSeverity(severity);
    }

    if (files.size()>1) {
        outputMsg << "[" << numCorrect << "/" << files.size() << "] ";
    } else {
        outputMsg << numCorrect;
    }

    outputMsg << " file" << ((numCorrect!=1) ? "s" : "") << " added to the tracker";
    result.setMessage(outputMsg.str());
}

/**
 * Adds the given file to the tracker
 *
 * @param file the file to add
 * @param result the result of the operation
 */
void PropsFileTracker::addFile(PropsFile &file, Result &result) {

    std::string fullFilePath = FileUtils::getAbsolutePath(file.getFileName());

    if (FileUtils::fileExists(fullFilePath)) {

        file.setFileName(fullFilePath);

        // make it master if none present
        if (getMasterFile() == nullptr) {
            file.setMaster(true);
        }

        result = storeFile(file);

        // Update the config
        if (result.isValid()) {
            result = save();
        }
    } else {
        result = res::ERROR;
        result.setMessage("File \"" + file.getFileName() + "\" cannot be read");
    }
}

/**
 * Removes the given file from the tracker
 *
 * @param file the file to remove
 */
void PropsFileTracker::removeFile(const std::string &file, Result& result) {
    std::string fullFilePath = FileUtils::getAbsolutePath(file);
    result = res::ERROR;
    if (trackedMapFiles_.count(fullFilePath) != 0) {
        auto* pFile = trackedMapFiles_[fullFilePath];
        trackedMapFiles_.erase(fullFilePath);
        aliasedMapFiles_.erase(pFile->getAlias());
        removeFileFromGroup(pFile);
        trackedFiles_.remove(*pFile);
        result = save();

        // Update the config
        if (result.isValid()) {
            result.setMessage("File \""+file+"\" removed from tracker");
        }
    } else {
        result = res::ERROR;
        result.setMessage("File \""+file+"\" is not tracked");
    }
}

/**
 * Removes the given file from the tracker using the alias
 *
 * @param file the alias of the file to remove
 */
void PropsFileTracker::removeFileByAlias(const std::string &fileAlias, Result& result) {
    result = res::ERROR;
    if (aliasedMapFiles_.count(fileAlias) != 0) {
        auto* pFile = aliasedMapFiles_[fileAlias];
        auto file = pFile->getFileName();
        aliasedMapFiles_.erase(fileAlias);
        trackedMapFiles_.erase(pFile->getFileName());
        removeFileFromGroup(pFile);
        trackedFiles_.remove(*pFile);
        result = save();

        // Update the config
        if (result.isValid()) {
            result.setMessage("File \""+file+"\" removed from tracker");
        }
    } else {
        result = res::ERROR;
        result.setMessage("Cannot find file with alias \"" + fileAlias + "\"");
    }
}

/**
 * Retrieves the list of currently tracked files
 * using the given output stream.
 *
 * @param output the output stream
*/
void PropsFileTracker::listTracked(std::ostream& output) const {
    if (trackedFiles_.empty()) {
        output << std::endl << rang::fgB::yellow << "No files tracked" << rang::fg::reset << std::endl;
    } else {
        // get max file size
        size_t maxFileNameSize = 0;
        for (auto& file : trackedMapFiles_) {
            std::string fileName = file.second->isMaster() ? "(M) " + file.first : file.first;
            maxFileNameSize = (maxFileNameSize < fileName.size()) ? fileName.size() : maxFileNameSize;
        }

        if (!trackedFiles_.empty()) {
            output << rang::fgB::green << "\n " << trackedFiles_.size() << " file" << ((trackedFiles_.size()!=1) ? "s" : "") << " tracked";
            // Hide group count if only 1 group, default group
            if (!((trackedGroups_.size() == 1) && (trackedGroups_.count(tracker::DEFAULT_GROUP) != 0))) {
                output << ", " << trackedGroups_.size() << " group" << ((trackedGroups_.size() != 1) ? "s" : "");
            }
            output << rang::fg::reset << std::endl;
        }

        for (auto& trackedGroup : trackedGroups_) {
            printTrackedGroup(output, maxFileNameSize, trackedGroup.first);
        }

        if (!trackedFiles_.empty()) {
            output << std::endl;
        }
    }
}

/**
 * Print the tracked
 * @param output
 * @param maxFileNameSize
 * @param trackedFiles
 * @return
 */
void PropsFileTracker::printTrackedGroup(std::ostream &output, size_t maxFileNameSize, const std::string& groupName) const {
    size_t i = 0;
    if (trackedGroups_.count(groupName) != 0) {
        const auto &trackedFiles = trackedGroups_.at(groupName);

        // Hide title if only 1 group, default group
        if (!((trackedGroups_.size() == 1) && (trackedGroups_.count(tracker::DEFAULT_GROUP) != 0))) {
            output << rang::fgB::blue << "\n " << ((groupName == tracker::DEFAULT_GROUP) ? groupName.substr(1, groupName.size()-1) : groupName) << rang::fg::reset;
        }

        for (auto &propsFile : trackedFiles) {
            bool last = (i == trackedFiles.size() - 1);
            std::string masterDetail = propsFile->getFileName();

            output << std::endl << " " << (last ? "└" : "├") << "─ ";
            if (propsFile->isMaster()) {
                output << rang::style::bold;
                masterDetail.append(" (M)");
            }
            output << masterDetail << rang::style::reset << rang::fg::reset;

            const auto &alias = propsFile->getAlias();
            std::string padding = (masterDetail.size() == maxFileNameSize) ? "" : StringUtils::padding(" ",
                                                                                                       maxFileNameSize -
                                                                                                       masterDetail.size());

            output << rang::fgB::yellow << (alias.empty() ? "" : padding.append(" => \"") + alias + "\"")
                   << rang::fg::reset;
            i++;
        }

        output << std::endl;
    }
}

/**
 * Sets the alias for a given file
 *
 * @param fileName the file to alias
 * @param fileAlias the alias to be set
 * @return the result of the operation
 */
Result PropsFileTracker::setAlias(const std::string& fileName, const std::string &fileAlias) {
    Result res{res::VALID};

    std::string fullFilePath = FileUtils::getAbsolutePath(fileName);

    PropsFile* propsFile = getFile(fullFilePath);

    if (propsFile != nullptr) {
        if (getFileWithAlias(fileAlias) != nullptr) {
            res = res::ERROR;
            res.setSeverity(res::CRITICAL);
            res.setMessage("The alias \"" + fileAlias + "\" is already used");
        } else {
            propsFile->setAlias(fileAlias);
            res = save();
            if (res.isValid()) {
                res.setMessage("Alias \"" + fileAlias + "\" set for file \"" + fileName + "\"");
            }
        }
    } else {
        res = res::ERROR;
        res.setSeverity(res::CRITICAL);
        res.setMessage("File \"" + fileName + "\" not tracked");
    }

    return res;
}

/**
 * Removes the given alias from the file.
 *
 * @param alias alias to remove
 */
Result PropsFileTracker::removeAlias(const std::string& alias) {
    Result res{res::VALID};
    PropsFile* propsFile = getFileWithAlias(alias);
    if (propsFile != nullptr) {
        propsFile->setAlias("");
        aliasedMapFiles_.erase(alias);
        res = save();
        if (res.isValid()) {
            res.setMessage("Alias \""+alias+"\" removed");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Alias \""+alias+"\" not found");
    }

    return res;
}

/**
 * Removes the current alias from the file.
 *
 * @param fileName the file to remove the alias from
 * @return the result of the operation
 */
Result PropsFileTracker::removeFileAlias(const std::string& fileName) {
    Result res{res::VALID};

    std::string fullFilePath = FileUtils::getAbsolutePath(fileName);

    PropsFile* propsFile = getFile(fullFilePath);
    
    if (propsFile != nullptr) {
        const std::string alias = propsFile->getAlias();
        if (alias.empty()) {
            res = res::ERROR;
            res.setSeverity(res::WARN);
            res.setMessage("File \"" + fileName + "\" not aliased");
        } else {
            propsFile->setAlias("");
            res = save();
            if (res.isValid()) {
                res.setMessage("Alias \""+alias+"\" removed");
            }
        }
    } else {
        res = res::ERROR;
        res.setSeverity(res::CRITICAL);
        res.setMessage("File \"" + fileName + "\" not tracked");
    }

    return res;
    
}

/**
 * Retrieves the file associated with the given alias
 * or null if not found.
 *
 * @param alias the alias
 * @return the props file or null if not found
 */
PropsFile* PropsFileTracker::getFileWithAlias(const std::string& alias) {
    PropsFile* propsFile = nullptr;
    if (aliasedMapFiles_.count(alias)!=0) {
        propsFile = aliasedMapFiles_[alias];
    }

    return propsFile;
}

/**
 * Retrieves the file with the given name
 * or null if not found.
 *
 * @param file the file name
 * @return the props file or null if not found
 */
PropsFile* PropsFileTracker::getFile(const std::string& file) {
    PropsFile* propsFile = nullptr;
    if (trackedMapFiles_.count(file)!=0) {
        propsFile = trackedMapFiles_[file];
    }

    return propsFile;
}

/**
 * Checks the file is valid and keeps a reference
 * in the tracker.
 *
 * @param propsFile the properties to store in the tracker
 */
Result PropsFileTracker::storeFile(PropsFile &propsFile) {

    Result result{res::ERROR};
    result.setSeverity(res::WARN);

    // Skip the file if not existing
    if (FileUtils::fileExists(propsFile.getFileName())) {
        if (trackedMapFiles_.count(propsFile.getFileName()) == 0) {
                if (maxTrackedFiles_ > trackedFiles_.size()) {
                    if (aliasedMapFiles_.count(propsFile.getAlias()) == 0) {
                        // Keep the file in the list
                        trackedFiles_.push_back(propsFile);

                        PropsFile *filePtr = &trackedFiles_.back();

                        // Keep a reference for the alias and the file for fast access
                        trackedMapFiles_[propsFile.getFileName()] = filePtr;
                        if (!propsFile.getAlias().empty()) {
                            aliasedMapFiles_[propsFile.getAlias()] = filePtr;
                        }

                        // Keep a reference in the group
                        std::string groupName = normalizeGroup(propsFile.getGroup());

                        if (trackedGroups_.count(groupName) == 0) {
                            trackedGroups_[groupName] = std::list<PropsFile*>{};
                        }

                        trackedGroups_[groupName].push_back(filePtr);


                        // Set as new master (override)
                        if (filePtr->isMaster()) {
                            updateMasterFile(filePtr);
                        }

                        result = res::VALID;
                        result.setSeverity(res::NORMAL);
                    } else {
                        result.setMessage("Alias [" + propsFile.getAlias() + "] already in use");
                    }
                } else {
                    result.setMessage("Cannot add file [" + propsFile.getFileName() +
                                      "], maximum number of tracked files reached (" +
                                      std::to_string(maxTrackedFiles_) + ")");
                }
        } else {
            result.setMessage("File [" + propsFile.getFileName() + "] already tracked");
        }
    } else {
        result.setMessage("File [" + propsFile.getFileName() + "] cannot be read");
    }

    return result;
}

/**
 * Stores the current configuration of the tracker
 *
 * @return the result of the operation
 */
Result PropsFileTracker::save() {
    Result res{res::VALID};
    try {
        updateTrackerConfig();
    } catch (std::exception& e) {
        res = res::ERROR;
        res.setSeverity(res::CRITICAL);
        res.setMessage(e.what());
    }

    return res;
}

/**
 * Removes all currently tracked files
 *
 * @return the result of the operation
 */
Result PropsFileTracker::clear() {
    Result res{res::VALID};

    if (trackedFiles_.empty()) {
        res.setSeverity(res::WARN);
        res.setMessage("No files currently tracked");
    } else {
        trackedGroups_.clear();
        aliasedMapFiles_.clear();
        trackedMapFiles_.clear();

        trackedFiles_.clear();

        res = save();

        if (res.isValid()) {
            res.setMessage("Removed all tracked files");
        }
    }

    return res;
}

/**
 * Updates the tracker config file with the given tracked file.
 * In case the config file is not detected a full dump is performed.
 *
 * @param propsFile the properties file to track
 */
void PropsFileTracker::updateTrackerConfig() const {

    auto configFilePath           = config::CONFIG_FULL_PATH() + tracker::TRACKER_CONFIG_FILE_NAME;
    std::string configFilePathTmp = config::CONFIG_FULL_PATH() + "." + tracker::TRACKER_CONFIG_FILE_NAME + ".tmp";
    std::string outputFilePath    = configFilePath;

    if (FileUtils::fileExists(configFilePath)) {
        outputFilePath = configFilePathTmp;
    }

    // Perform a dump of the current config (existing extra comments are lost :()
    writeTrackerConfig(outputFilePath);

    // Rename temporary file if needed
    if (outputFilePath == configFilePathTmp) {
        if (!FileUtils::rename(configFilePathTmp, configFilePath)) {
            FileUtils::remove(configFilePathTmp);
            throw ExecutionException("I/O Error updating tracker configuration file");
        }
    }
}

/**
 * Writes the tracker current configuration to the default output file
 * under user's home directory.
 */
void PropsFileTracker::writeTrackerConfig(const std::string& outputFilePath) const {

    if (!FileUtils::fileExists(outputFilePath)) {

        FileUtils::createDirectories(outputFilePath);

        std::ofstream outFile(outputFilePath);
        if (outFile.is_open()) {

            const std::string& spacer = StringUtils::padding(" ", 8);

            outFile << "[Tracking]\nfiles = [";
            std::string prefix;
            for (auto &propFile : trackedFiles_) {
                outFile << prefix << " {"
                        << (!propFile.getAlias().empty() ? "alias = \"" + propFile.getAlias() + "\", " : "")
                        << "location = \"" + propFile.getFileName() + "\""
                        << (propFile.isMaster() ? ", master = true" : "")
                        << (!propFile.getGroup().empty() ? ", group = \"" + propFile.getGroup() + "\"" : "") << "}";
                prefix = ",\n"+ spacer + " ";
            }
            outFile << ((trackedFiles_.size()>1) ? "\n" + spacer : " ") << "]" << std::endl;
            outFile.close();
        } else {
            throw ExecutionException("Cannot write tracker config file");
        }
    }
}

/**
 * Removes the given file from its group (if any).
 *
 * @param propsFile the props file
 */
void PropsFileTracker::removeFileFromGroup(PropsFile* propsFile) {
    if (propsFile != nullptr) {
        std::string groupName = normalizeGroup(propsFile->getGroup());

        if ((groupName != tracker::DEFAULT_GROUP) && (trackedGroups_.count(groupName) != 0)) {
            trackedGroups_.at(groupName).remove(propsFile);
            propsFile->setGroup("");
        }
    }
}

/**
 * Moves the object specified by its file/alias
 * to the target group. If the group does not
 * exist it is created automatically.
 *
 * @param name the file name or alias
 * @param targetGroup the target group
 * @param isAlias true if the name is an alias
 * @return the result of the operation
 */
Result PropsFileTracker::group(const std::string& name, const std::string& targetGroup, const bool& isAlias) {
    Result res{res::VALID};
    std::string nTrgGroup = normalizeGroup(targetGroup);

    PropsFile* file = (isAlias) ? getFileWithAlias(name) : getFile(FileUtils::getAbsolutePath(name));

    if (file != nullptr) {
        if (normalizeGroup(file->getGroup()) != nTrgGroup) {
            // Insert file in target group creating the group if not existing
            if (trackedGroups_.count(nTrgGroup) == 0) {
            } else {
                trackedGroups_[nTrgGroup] = std::list<PropsFile *>{};
            }
            trackedGroups_[nTrgGroup].push_back(file);

            // Switch file groups
            const std::string &previousGroup = normalizeGroup(file->getGroup());
            if (trackedGroups_.count(previousGroup) != 0) {
                trackedGroups_[previousGroup].remove(file);
                file->setGroup(nTrgGroup);
            }
            res = save();
            if (res.isValid()) {
                res.setMessage("File \"" + file->getFileName() + "\" moved to group \"" + (nTrgGroup == tracker::DEFAULT_GROUP ? nTrgGroup.erase(0,1) : nTrgGroup) + "\"");
            }
        } else {
            res = res::ERROR;
            res.setSeverity(res::WARN);
            res.setMessage("File \"" + file->getFileName() + "\" already in group \"" + (nTrgGroup == tracker::DEFAULT_GROUP ? nTrgGroup.erase(0,1) : nTrgGroup) + "\"");
        }
    } else {
        res = res::ERROR;
        res.setMessage(std::string((isAlias ? "Alias" : "File")) + " \"" + name + "\" not found");
    }

    return res;
}

/**
 * Removes the group effectively moving all its
 * contained files to the default group.
 *
 * @param group the group to remove
 * @return the result of the operation
 */
Result PropsFileTracker::removeGroup(const std::string& group) {
    Result res{res::VALID};
    const std::string nGroup = normalizeGroup(group);

    if (nGroup != tracker::DEFAULT_GROUP) {
        const std::list<PropsFile *> *groups = getGroup(nGroup);

        if (groups != nullptr) {
            for (auto &propsFile : *groups) {
                propsFile->setGroup("");
            }
            trackedGroups_.erase(nGroup);
            res = save();
            if (res.isValid()) {
                res.setMessage("Group \"" + nGroup + "\" removed from tracker");
            }
        } else {
            res = res::ERROR;
            res.setMessage("Group \"" + nGroup + "\" not found");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Cannot remove default group");
    }

    return res;
}

/**
 * Removes the group effectively moving all its
 * contained files to the default group.
 *
 * @param group the group to remove
 * @return the result of the operation
 */
Result PropsFileTracker::renameGroup(const std::string& sourceGroup, const std::string& targetGroup, const bool& force) {
    Result res{res::VALID};

    std::string nSrcGroup = normalizeGroup(sourceGroup);
    std::string nTrgGroup = normalizeGroup(targetGroup);

    if (nSrcGroup != tracker::DEFAULT_GROUP) {

        const std::list<PropsFile *> *files = getGroup(nSrcGroup);

        if (files != nullptr) {
            if (nSrcGroup != nTrgGroup) {
                if ((trackedGroups_.count(nTrgGroup) == 0) || force) {
                    if (trackedGroups_.count(nTrgGroup) == 0) {
                        trackedGroups_[nTrgGroup] = std::list<PropsFile *>{};
                    }
                    for (auto &propsFile : *files) {
                        propsFile->setGroup(nTrgGroup);
                        trackedGroups_[nTrgGroup].push_back(propsFile);
                    }
                    trackedGroups_.erase(nSrcGroup);
                    res = save();
                    if (res.isValid()) {
                        res.setMessage("Group \"" + nSrcGroup + "\" renamed to \"" + targetGroup + "\"");
                    }
                } else {
                    res = res::ERROR;
                    res.setMessage("Target group \"" + targetGroup + "\" already exists");
                }
            } else {
                res = res::ERROR;
                res.setMessage("Same source and target groups specified");
            }
        } else {
            res = res::ERROR;
            res.setMessage("Group \"" + nSrcGroup + "\" not found");
        }
    } else {
        res = res::ERROR;
        res.setMessage("Cannot rename default group");
    }

    return res;
}


