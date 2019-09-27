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
#include <pcrecpp.h>
#include <string_utils.h>
#include <parser/toml.hpp>

// Prototypes for globals
const pcrecpp::RE &SECTION_LINE();

/**
 * Retrieves the regular expression for
 * section lines.
 *
 * @return the regex for section lines
 */
const pcrecpp::RE& SECTION_LINE() {
    static const pcrecpp::RE SECTION_LINE(R"(^[\s]*\[(.*)\][\s]*$)");
    return SECTION_LINE;
}

/**
 * Namespace for constants
 */
namespace tracker {
    const std::string& CONFIG_FOLDER_PATH();
    const std::string& PROPS_FOLDER();
    const std::string& CONFIG_FULL_PATH();
    const std::string& CONFIG_FILE_PATH();

    static const char* CONFIG_FILE_NAME = "props-tracker.conf";
    static const int DEFAULT_MAX_TRACKED_FILES = 20; // TODO configurable in .propsrc

    // The path to the config folder
    const std::string& CONFIG_FOLDER_PATH() {
        static const std::string CONFIG_FOLDER_PATH =
                FileUtils::getHomeDir() + ftl::pathSeparator + ".config" + ftl::pathSeparator;
        return CONFIG_FOLDER_PATH;
    }

    // The props folder name
    const std::string& PROPS_FOLDER() {
        static const std::string PROPS_FOLDER = std::string("props") + ftl::pathSeparator;
        return PROPS_FOLDER;
    }

    const std::string& CONFIG_FULL_PATH() {
        static const std::string CONFIG_FULL_PATH = CONFIG_FOLDER_PATH() + PROPS_FOLDER();
        return CONFIG_FULL_PATH;
    }
    const std::string& CONFIG_FILE_PATH() {
        static const std::string CONFIG_FILE_PATH = CONFIG_FULL_PATH() + CONFIG_FILE_NAME;
        return CONFIG_FILE_PATH;
    }
}

/**
 * Default constructor. Reads tracker config
 * if available.
 */
PropsFileTracker::PropsFileTracker() {
    //TODO Replace by PropsConfig::getValue(config::MAX_TRACKED_FILES, DEFAULT_MAX_TRACKED_FILES)
    maxTrackedFiles_ = tracker::DEFAULT_MAX_TRACKED_FILES;
    parseTrackerConfig();
};

/**
 * Parses the tracker current configuration from the default
 * user's config file.
 */
void PropsFileTracker::parseTrackerConfig() {
    auto& configFilePath = tracker::CONFIG_FILE_PATH();
    std::string masterFileName;
    Result result{res::VALID};

    try {
        const auto data = toml::parse(configFilePath);

        const auto& generalSection = toml::find(data, "General");
        masterFileName = toml::find<std::string>(generalSection, "master");

        toml::value trackingSection = toml::find<toml::value >(data, "Tracking");
        toml::array fileArray = toml::find<toml::array>(trackingSection, "files");
        for (auto& file : fileArray) {
            toml::value fileTable = toml::get<toml::value >(file);

            const auto alias = toml::find_or<std::string>(fileTable, "alias", "");
            const auto& location = toml::find<std::string>(fileTable, "location");
            // Creates the new props file
            PropsFile propsFile;
            propsFile.setFileName(location);
            propsFile.setAlias(alias);
            propsFile.setMaster(false);
            result = storeFile(propsFile);
        }
    } catch (std::exception& e) {
        throw ExecutionException("Error parsing configuration file. Details : "+ std::string(e.what()));
    }

    // Sets the master
    if (!StringUtils::isEmptyOrBlank(masterFileName)) {
        result = storeMaster(masterFileName);
    } else if (!trackedFiles_.empty()){
        // no master ? take first
        auto& front = trackedFiles_.front();
        front.setMaster(true);
        setMasterFile(&front);
    }

    result.showMessage();
}

/**
 * Sets the file as master and automatically tracks it
 * if not found.
 *
 * @param masterFileName the master file name
 * @return the result of the operation
 */
Result PropsFileTracker::storeMaster(const std::string &masterFileName) {
    Result result{res::VALID};
    if (trackedMapFiles_.count(masterFileName) != 0) {
        trackedMapFiles_[masterFileName]->setMaster(true);
        masterFile_ = trackedMapFiles_[masterFileName];
    } else {
        PropsFile propsFile;
        propsFile.setMaster(true);
        propsFile.setFileName(masterFileName);
        result = storeFile(propsFile);
        setMasterFile(&propsFile);

        if (result.isValid()) {
            // Inconsistency detected, the master file was not present in the list of tracked ones
            // emit a warning and proceed keeping it in the list
            result.setMessage("WARN: Master file [" + masterFileName
                      + "] not in tracked list. Add it to the [Tracking] section to avoid this warning");
            result.setSeverity(res::WARN);
        }
    }

    return result;
}

/**
 * Adds the given file to the tracker
 *
 * @param file the file to add
 */
void PropsFileTracker::addFile(PropsFile& file, Result& result) {

    if ((maxTrackedFiles_ < 0) || (trackedFiles_.size() < maxTrackedFiles_)) {
        std::string fullFilePath = FileUtils::getAbsolutePath(file.getFileName());

        if (FileUtils::fileExists(fullFilePath)) {
            file.setFileName(fullFilePath);
            // make it master if absent
            if (getMasterFile() == nullptr) {
                setMasterFile(&file);
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
    } else {
        throw InitializationException("The maximum number of tracked files has been reached ("+ std::to_string(trackedFiles_.size()) +"]");
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
void PropsFileTracker::listTracked(std::ostream& output) {
    if (trackedFiles_.empty()) {
        output << rang::fgB::yellow << trackedFiles_.size() << "No files tracked" << rang::fg::reset << std::endl;
    } else {
        output << rang::fgB::green << "Files currently tracked :\n" << rang::fg::reset << std::endl;
        int counter = 1;
        for (auto &propsFile : trackedFiles_) {
            auto &alias = propsFile.getAlias();
            output << rang::style::bold << "\t[" << counter << "] : " << rang::style::reset << rang::fgB::cyan << (!alias.empty() ? "\"" + alias + "\" => " : "")
                   << rang::fg::reset   << propsFile.getFileName() << (propsFile.isMaster() ? " (M)" : "") << std::endl;
        }
    }
}


/**
 * Checks the file is valid and keeps a reference
 * in the tracker.
 *
 * @param propsFile the properties to store in the tracker
 */
Result PropsFileTracker::storeFile(PropsFile& propsFile) {

    Result result{res::ERROR};
    result.setSeverity(res::WARN);

    // Skip the file if not existing
    if (FileUtils::fileExists(propsFile.getFileName())) {
        if (trackedMapFiles_.count(propsFile.getFileName()) == 0) {
            if (aliasedMapFiles_.count(propsFile.getAlias()) == 0) {
                // Keep the file in the list
                trackedFiles_.push_back(propsFile);

                PropsFile* filePtr = &trackedFiles_.back();

                // Keep a reference for the alias and the file for fast access
                trackedMapFiles_[propsFile.getFileName()] = filePtr;
                if (!propsFile.getAlias().empty()) {
                    aliasedMapFiles_[propsFile.getAlias()] = filePtr;
                }

                // Set as new master (override)
                if (propsFile.isMaster()) {
                    setMasterFile(&propsFile);
                }

                result = res::VALID;
                result.setSeverity(res::NORMAL);
            } else {
                result.setMessage("WARN: Alias [" + propsFile.getAlias() + "] already in use. Skipping");
            }
        } else {
            result.setMessage("WARN: File [" + propsFile.getFileName() + "] already tracked. Skipping");
        }
    } else {
        result.setMessage("WARN: File [" + propsFile.getFileName() + "] cannot be read. Skipping");
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

    auto& configFilePath          = tracker::CONFIG_FILE_PATH();
    std::string configFilePathTmp = tracker::CONFIG_FULL_PATH() + "." + tracker::CONFIG_FILE_NAME + ".tmp";
    std::string outputFilePath    = tracker::CONFIG_FILE_PATH();;

    if (FileUtils::fileExists(configFilePath)) {
        outputFilePath = configFilePathTmp;
    }

    // Perform a dump of the current config (existing extra comments are lost :()
    writeTrackerConfig(outputFilePath);

    // Rename temporary file if needed
    if (outputFilePath == configFilePathTmp) {
        if (!FileUtils::rename(configFilePathTmp, configFilePath)) {
            remove(configFilePathTmp);
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
            outFile << "[General]";
            if (getMasterFile() != nullptr) {
                outFile << "\nmaster = \"" << getMasterFile()->getFileName() + "\"\n";
            }
            outFile << "\n[Tracking]\nfiles = [";
            std::string prefix;
            for (auto &propFile : getTrackedFiles()) {
                outFile << prefix << "\n\t{"
                        << (!propFile.getAlias().empty() ? "alias = \"" + propFile.getAlias() + "\", " : "")
                        << "location = \"" + propFile.getFileName() + "\"}";
                prefix = ",";
            }
            outFile << "\n]" << std::endl;
            outFile.close();
        } else {
            throw ExecutionException("Cannot write tracker config file");
        }
    }
}


