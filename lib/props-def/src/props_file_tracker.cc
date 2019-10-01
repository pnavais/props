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
 * Namespace for constants
 */
namespace tracker {
    static const char* TRACKER_CONFIG_FILE_NAME = "props-tracker.conf";
    static const long DEFAULT_MAX_TRACKED_FILES = 20;
    static const char* MAX_TRACKED_FILES = "general.max_tracked_files";
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
                const auto &location = toml::find<std::string>(fileTable, "location");

                // Creates the new props file
                PropsFile propsFile;
                propsFile.setFileName(location);
                propsFile.setAlias(alias);
                propsFile.setMaster(master);
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
 * Adds the given file to the tracker
 *
 * @param file the file to add
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
            updateTrackerConfig();
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
        trackedFiles_.remove(*pFile);
        result = res::VALID;
        result.setMessage("File \""+file+"\" removed from tracker");

        // Update the config
        if (result.isValid()) {
            updateTrackerConfig();
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
        aliasedMapFiles_.erase(fileAlias);
        trackedMapFiles_.erase(pFile->getFileName());
        trackedFiles_.remove(*pFile);
        result = res::VALID;

        // Update the config
        if (result.isValid()) {
            updateTrackerConfig();
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
            output << rang::fgB::green << "\n " << trackedFiles_.size() << " file" << ((trackedFiles_.size()!=1) ? "s" : "") << " tracked" << rang::fg::reset << std::endl;
        }

        size_t i=0;
        for (auto& propsFile : trackedFiles_) {
            bool last = (i==trackedFiles_.size()-1);
            std::string masterDetail = propsFile.getFileName();
            if (propsFile.isMaster()) {
                output << rang::style::bold;
                masterDetail.append(" (M)");
            }
            output << std::endl << " " << (last ? "└" : "├") << "─ " << masterDetail << rang::style::reset << rang::fg::reset;

            const auto& alias = propsFile.getAlias();
            std::string padding = (masterDetail.size() == maxFileNameSize) ? "" :  StringUtils::padding(" ", maxFileNameSize-masterDetail.size());

            output << rang::fgB::yellow << (alias.empty() ? "" : padding.append(" => \"") + alias + "\"")
                   << rang::fg::reset;
            i++;
        }

        if (!trackedFiles_.empty()) {
            output << std::endl;
        }

    }
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
        updateTrackerConfig();
        res.setMessage("Alias \""+alias+"\" removed");
    } else {
        res.setSeverity(res::WARN);
        res.setMessage("Alias \""+alias+"\" not found");
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
                        << (propFile.isMaster() ? ", master = true" : "") << "}";
                prefix = ",\n"+ spacer + " ";
            }
            outFile << ((trackedFiles_.size()>1) ? "\n" + spacer : " ") << "]" << std::endl;
            outFile.close();
        } else {
            throw ExecutionException("Cannot write tracker config file");
        }
    }
}


