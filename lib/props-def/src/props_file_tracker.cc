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
#include <fstream>
#include <exec_exception.h>
#include <init_exception.h>
#include <pcrecpp.h>
#include <string_utils.h>

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

    Result result{res::VALID};
    std::string currentSection;
    std::string masterFileName;
    std::string alias;
    std::string aliasGroup;
    std::string fileName;

    std::ifstream infile(configFilePath);

    // File exists, parse it to obtain the tracked files
    if (infile) {
        std::string line;
        while (std::getline(infile, line)) {
            if (!SECTION_LINE().FullMatch(line, &currentSection)) {
                if (currentSection == "General") {
                    pcrecpp::RE(R"(^[\s]*master=(.+)$)").FullMatch(line, &masterFileName);
                } else if (currentSection == "Tracked Files") {
                    if (pcrecpp::RE(R"(^[\s]*(([^=]*)=){0,1}(.+)[\s]*$)").FullMatch(line, &aliasGroup, &alias, &fileName)) {
                        // Creates the new props file
                        PropsFile propsFile;
                        fileName = (fileName.empty()) ? alias : fileName;
                        propsFile.setFileName(fileName);
                        propsFile.setAlias(alias);
                        propsFile.setMaster(false);

                        result = storeFile(propsFile);
                    }
                }
            }
        }

        // Sets the master
        if (!StringUtils::isEmptyOrBlank(masterFileName)) {
            result = storeMaster(masterFileName);
        } else if (!trackedFiles_.empty()){
            // no master ? take first
            setMasterFile(trackedFiles_.front());
        }

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
        trackedMapFiles_[masterFileName].get()->setMaster(true);
    } else {
        PropsFile propsFile;
        propsFile.setMaster(true);
        propsFile.setFileName(masterFileName);
        result = storeFile(propsFile);

        if (result.isValid()) {
            // Inconsistency detected, the master file was not present in the list of tracked ones
            // emit a warning and proceed keeping it in the list
            result.setMessage("WARN: Master file [" + masterFileName
                      + "] not in tracked list. Add it to the [Tracked Files] section to avoid this warning");
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
                setMasterFile(file);
            }

            result = storeFile(file);

            // Update the config
            if (result.isValid()) {
                updateTrackerConfig(file);
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
        auto* pFile = trackedMapFiles_[fullFilePath].get();
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
        auto* pFile = aliasedMapFiles_[fileAlias].get();
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
    for (auto& propsFile : trackedFiles_) {
        auto& alias = propsFile.getAlias();
        output << (!alias.empty() ? "[" + alias + "] -> " : "")
                  << propsFile.getFileName() << (propsFile.isMaster() ? "(M)" : "") << std::endl;
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
                auto filePtr = std::make_shared<PropsFile>(propsFile);

                // Keep a reference for the alias and the file for fast access
                trackedMapFiles_[propsFile.getFileName()] = filePtr;
                if (!propsFile.getAlias().empty()) {
                    aliasedMapFiles_[propsFile.getAlias()] = filePtr;
                }

                // Set as new master (override)
                if (propsFile.isMaster()) {
                    setMasterFile(propsFile);
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
void PropsFileTracker::updateTrackerConfig(const PropsFile &propsFile) const {

    auto& configFilePath = tracker::CONFIG_FILE_PATH();
    std::string configFilePathTmp = tracker::CONFIG_FULL_PATH() + "." + tracker::CONFIG_FILE_NAME + ".tmp";
    if (FileUtils::fileExists(configFilePath)) {
        std::ofstream outFile(configFilePathTmp);
        std::ifstream inFile(configFilePath);

        // File exists, so parse it to obtain the tracked files
        if (inFile && (outFile.is_open())) {
            std::string line;
            std::string currentSection;
            std::string previousSection;
            bool generalFound = false;
            bool trackedFound = false;
            bool masterReplaced = false;
            bool fileTacked = false;

            while (!inFile.eof()) {

                std::getline(inFile, line);

                std::string beforeSection = currentSection;
                bool newSection = SECTION_LINE().FullMatch(line, &currentSection);
                if (beforeSection != currentSection) {
                    previousSection = beforeSection;
                }

                if (currentSection == "General") {
                    generalFound = true;
                    if (propsFile.isMaster() && !newSection && (pcrecpp::RE(R"((^[\s]*master=)(.+)$)").Replace("\\1" + propsFile.getFileName(), &line))) {
                        masterReplaced = true;
                    }
                } else if (!trackedFound && (currentSection == "Tracked Files")) {
                    trackedFound = true;
                }

                // If general found but new section reached without master replacement
                if (propsFile.isMaster() && (previousSection == "General") && newSection && !masterReplaced && generalFound) {
                    outFile << "master=" << propsFile.getFileName() << std::endl;
                    masterReplaced = true;
                }

                // If tracked found but new section reached
                if ((propsFile.isMaster() && (previousSection == "Tracked Files") && newSection && trackedFound) ||
                    (inFile.eof() && !fileTacked && trackedFound && currentSection == "Tracked Files")) {
                    outFile << (!propsFile.getAlias().empty() ? propsFile.getAlias() + "=" : "")
                            << propsFile.getFileName() << std::endl;
                    fileTacked = true;
                }

                if(!inFile.eof()) {
                    outFile << line << std::endl;
                }
            }

            // Create General section if not found and add the master file
            if (!masterReplaced) {
                if (propsFile.isMaster()) {
                    if (!generalFound) {
                        outFile << "[General]" << std::endl;
                    }
                    outFile << "master=" << propsFile.getFileName() << std::endl;
                }
            }

            // Create tracked files section if not found and add the file
            if (!fileTacked) {
                if (!trackedFound) {
                    outFile << "[Tracked Files]" << std::endl;
                }
                outFile << (!propsFile.getAlias().empty() ? propsFile.getAlias() + "=" : "")
                        << propsFile.getFileName() << std::endl;
            }

            outFile.close();
            inFile.close();

            if (!FileUtils::rename(configFilePathTmp, configFilePath)) {
                remove(configFilePathTmp);
                throw ExecutionException("I/O Error updating tracker configuration file");
            }
        } else {
            throw ExecutionException("Error updating tracker configuration file");
        }
    }  else {
        // Perform a full dump
        writeTrackerConfig();
    }
}

/**
 * Writes the tracker current configuration to the default output file
 * under user's home directory.
 */
void PropsFileTracker::writeTrackerConfig() const {
    auto& configFilePath = tracker::CONFIG_FILE_PATH();
    if (!FileUtils::fileExists(configFilePath)) {

        FileUtils::createDirectories(tracker::CONFIG_FULL_PATH());

        std::ofstream outFile(configFilePath);
        if (outFile.is_open()) {
            outFile << "[General]";
            if (getMasterFile() != nullptr) {
                outFile << "\nmaster=" << getMasterFile().get()->getFileName() + "\n";
            }
            outFile << "\n[Tracked Files]";
            for (auto &propFile : getTrackedFiles()) {
                outFile << "\n" << (!propFile.getAlias().empty() ? propFile.getAlias() + "=" : "")
                        << propFile.getFileName();
            }
            outFile << std::endl;
            outFile.close();
        } else {
            throw ExecutionException("Cannot write tracker config file");
        }
    }
}


