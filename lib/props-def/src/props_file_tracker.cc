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
#include <rang.hpp>

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

namespace tracker {
    const static std::string CONFIG_FOLDER_PATH = FileUtils::getHomeDir() + ftl::pathSeparator + ".config" + ftl::pathSeparator;
    const static std::string PROPS_FOLDER     = std::string("props") + ftl::pathSeparator;
    const static std::string OUTPUT_FILE_NAME = "props-tracker.conf";
    const static std::string CONFIG_FULL_FOLDER_PATH = CONFIG_FOLDER_PATH + PROPS_FOLDER;
    const static std::string CONFIG_FILE_PATH = CONFIG_FOLDER_PATH + PROPS_FOLDER + OUTPUT_FILE_NAME;
    const static int DEFAULT_MAX_TRACKED_FILES = 20; // TODO configurable in .propsrc
}

/**
 * Adds the given file to the tracker
 *
 * @param file the file to add
 */
void PropsFileTracker::addFile(PropsFile& file, Result& result) {

    // TODO Replace by PropsConfig::getValue(config::MAX_TRACKED_FILES, DEFAULT_MAX_TRACKED_FILES)
    if (trackedFiles_.size() < tracker::DEFAULT_MAX_TRACKED_FILES) {
        std::string fullFilePath = FileUtils::getAbsolutePath(file.getFileName());

        if (FileUtils::fileExists(fullFilePath)) {
            file.setFileName(fullFilePath);

            // Only include file if not previously tracked
            if (trackedMapFiles_.count(fullFilePath) == 0) {
                trackedFiles_.push_back(file);
                auto filePtr = std::make_shared<PropsFile>(file);

                // Keep the tracked file in the map and the alias if any
                trackedMapFiles_[fullFilePath] = filePtr;
                if (!file.getAlias().empty()) {
                    aliasedMapFiles_[file.getAlias()] = filePtr ;
                }

                // Set as new master (override)
                if (file.isMaster()) {
                    setMaster(file);
                }

                updateTrackerConfig(file);
            } else {
                result = res::ERROR;
                result.setSeverity(res::WARN);
                result.setMessage("The file \"" + file.getFileName() + "\" is already tracked");
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
 * Updates the tracker config file with the given tracked file.
 * In case the config file is not detected a full dump is performed.
 *
 * @param propsFile the properties file to track
 */
void PropsFileTracker::updateTrackerConfig(const PropsFile &propsFile) const {
    if (FileUtils::fileExists(tracker::CONFIG_FILE_PATH)) {
        std::cout << "Updating file " << propsFile.getFileName() << "!!!" << std::endl;
    } else {
        // Full dump
        writeTrackerConfig();
    }
}

/**
 * Writes the tracker current configuration to the default output file
 * under user's home directory.
 */
void PropsFileTracker::writeTrackerConfig() const {
    if (!FileUtils::fileExists(tracker::CONFIG_FILE_PATH)) {

        FileUtils::createDirectories(tracker::CONFIG_FULL_FOLDER_PATH);
        std::ofstream outFile(tracker::CONFIG_FILE_PATH);
        if (outFile.is_open()) {
            outFile << "[General]";
            outFile << "\n\tmaster=" << getMaster().get()->getFileName() + "\n";
            outFile << "\n[Tracked Files]";
            for (auto &propFile : getAll()) {
                outFile << "\n\t" << (!propFile.getAlias().empty() ? propFile.getAlias() + "=" : "")
                        << propFile.getFileName();
            }
            outFile << std::endl;
            outFile.close();
        } else {
            throw ExecutionException("Cannot write tracker config file");
        }
    }
}

/**
 * Parses the tracker current configuration from the default
 * user's config file.
 */
void PropsFileTracker::parseTrackerConfig() {
    std::string configFilePath = tracker::CONFIG_FILE_PATH;

    std::string currentSection;
    std::string masterFileName;
    std::string alias;
    std::string fileName;

    std::ifstream infile(configFilePath);

    // File exists, so parse it to obtain the tracked files
    if (infile) {
        std::string line;
        while (std::getline(infile, line)) {
            if (!SECTION_LINE().FullMatch(line, &currentSection)) {
                if (currentSection == "General") {
                    pcrecpp::RE(R"(^[\s]*master=(.+)$)").FullMatch(line, &masterFileName);
                } else if (currentSection == "Tracked Files") {
                    if (pcrecpp::RE(R"(^[\s]*([^=]*)={0,1}(.+)$)").FullMatch(line, &alias, &fileName)) {

                        // Creates the new props file
                        PropsFile propsFile;
                        fileName = (fileName.empty()) ? alias : fileName;
                        propsFile.setFileName(fileName);
                        propsFile.setAlias(alias);
                        propsFile.setMaster(false);

                        storeFile(propsFile);
                    }
                }
            }
        }

        // Sets the master
        if (trackedMapFiles_.count(masterFileName) != 0) {
            trackedMapFiles_[masterFileName].get()->setMaster(true);
        } else {
            // Inconsistency detected, the master file was not present in the list of tracked ones
            // emit a warning and proceed keeping it in the list
            std::cerr << rang::fgB::yellow << "WARN: Master file [" << masterFileName << "] not in tracked list. Add it to the [Tracked Files] section to avoid this warning" << rang::fg::reset << std::endl;
            PropsFile propsFile;
            propsFile.setMaster(true);
            propsFile.setFileName(masterFileName);
            storeFile(propsFile);
        }
    }
}

/**
 * Removes the given file from the tracker
 *
 * @param file the file to remove
 */
void PropsFileTracker::removeFile(const std::string &file, Result& result) {
    std::string fullFilePath = FileUtils::getAbsolutePath(file);
    if (std::find(trackedFiles_.begin(), trackedFiles_.end(), fullFilePath) != trackedFiles_.end()) {
        trackedFiles_.remove(PropsFile::make_file(fullFilePath));
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
    for (auto& propsFile : trackedFiles_) {
        if (propsFile.getAlias() == fileAlias) {
            trackedFiles_.remove(propsFile);
            result = res::VALID;
            break;
        }
    }

    if (result == res::ERROR) {
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
}

/**
 * Checks the file is valid and keeps a reference
 * in the tracker.
 *
 * @param propsFile the properties to store in the tracker
 */
void PropsFileTracker::storeFile(PropsFile& propsFile) {

    // Skip the file if not existing
    if (FileUtils::fileExists(propsFile.getFileName())) {

        if (trackedMapFiles_.count(propsFile.getFileName()) == 0) {

            // Keep the file in the list
            trackedFiles_.push_back(propsFile);
            auto filePtr = std::make_shared<PropsFile>(propsFile);

            // Keep a reference for the alias and the file for fast access
            trackedMapFiles_[propsFile.getFileName()] = filePtr;
            if (!propsFile.getAlias().empty()) {
                aliasedMapFiles_[propsFile.getAlias()] = filePtr;
            }
        } else {
            std::cerr << rang::fgB::yellow << "WARN: File [" << propsFile.getFileName() << "] already tracked. Skipping" << rang::fg::reset << std::endl;
        }
    } else {
        std::cerr << rang::fgB::yellow << "WARN: File [" << propsFile.getFileName() << "] cannot be read. Skipping" << rang::fg::reset << std::endl;
    }
}

