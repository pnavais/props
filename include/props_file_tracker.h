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

#ifndef PROPS_PROPS_FILE_TRACKER_H
#define PROPS_PROPS_FILE_TRACKER_H

#include <list>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>
#include "result.h"
#include "props_file.h"

class PropsFileTracker {

public:

    /**
     * Retrieves the singleton instance
     * @return the singleton instance
     */
    static PropsFileTracker &getDefault() {
        static PropsFileTracker instance;
        return instance;
    }

    /**
     * Adds a new file to the tracker
     *
     * @param file the file to add
     */
    static Result add(PropsFile &file) {
        Result result{res::VALID};
        getDefault().addFile(file, result);
        return result;
    }

    /**
     * Removes a tracked file using the file name
     *
     * @param the file to remove from the tracker
     */
    static Result remove(const std::string &filePath) {
        Result result{res::VALID};
        getDefault().removeFile(filePath, result);
        return result;
    }

    /**
    * Removes a tracked file using its alias
    *
    * @param the file to remove from the tracker
    */
    static Result removeByAlias(const std::string &fileAlias) {
        Result result{res::VALID};
        getDefault().removeFileByAlias(fileAlias, result);
        return result;
    }

    /**
     * Retrieve all tracked files
     *
     * @return the list of tracked files
     */
    const std::list<PropsFile>& getTrackedFiles() const {
        return getDefault().trackedFiles_;
    }

    /**
     * Sets the given file as master.
     *
     * @param propsFile the file to be set as master
     */
    void setMasterFile(const PropsFile &propsFile) {
        masterFile_ = std::make_shared<PropsFile>(propsFile);
    }

    /**
     * Retrieves the master file
     *
     * @return the master file
     */
    const std::shared_ptr<PropsFile> &getMasterFile() const {
        return masterFile_;
    }

    /**
     * Displays the list of currently tracked files
     * in the standard output
     */
    void listTracked() {
        listTracked(std::cout);
    }

    /**
     * Sets the maximum number of files to track.
     * NOTE : A negative value means no limit.
     *
     * @param maxTrackedFiles the maximum number of files to track
     */
    static void setMaxTrackedFiles(const unsigned long& maxTrackedFiles) {
        getDefault().maxTrackedFiles_ = maxTrackedFiles;
    }

    /**
    * Retrieves the maximum number of files to track.
    * NOTE : A negative value means no limit.
    *
    * @return the maximum number of files to track
    */
    static const unsigned long& getMaxTrackedFiles() {
        return getDefault().maxTrackedFiles_;
    }

    /**
    * Retrieves the list of currently tracked files
    * using the given output stream.
    */
    void listTracked(std::ostream &output);

private:

    /**
     * Default constructor
     */
    PropsFileTracker();

    /**
     * Parses the tracker current configuration from the default
     * user's config file.
     */
    void parseTrackerConfig();

    /**
     * Adds the given file to the tracker
     *
     * @param file the file to add
     */
    void addFile(PropsFile &file, Result &result);

    /**
     * Removes the given file from the tracker using the file path
     *
     * @param filePath the path to the file to remove
     */
    void removeFile(const std::string &filePath, Result &result);

    /**
    * Removes the given file from the tracker using the alias
    *
    * @param file the alias of the file to remove
    */
    void removeFileByAlias(const std::string &fileAlias, Result &result);

    /**
     * Checks the file is valid and keeps a reference
     * in the tracker.
     *
     * @param propsFile the properties to store in the tracker
     */
    Result storeFile(PropsFile& propsFile);

    /**
     * Updates the tracker config file with the given tracked file.
     * In case the config file is not detected a full dump is performed.
     *
     * @param propsFile the properties file to track
     */
    void updateTrackerConfig(const PropsFile &propsFile) const;

    /**
     * Writes the tracker current configuration to the default output file
     * under user's home directory.
     */
    void writeTrackerConfig() const;

    /**
     * Sets the file as master and automatically tracks it
     * if not found.
     *
     * @param masterFileName the master file name
     */
    Result storeMaster(const std::string &masterFileName);

    /** The master file */
    std::shared_ptr<PropsFile> masterFile_;

    /** The list of tracked files */
    std::list<PropsFile> trackedFiles_;

    /** The maximum number of files to track (Negative value means no limit) */
    unsigned long maxTrackedFiles_;

    /** The map of tracked files */
    std::map<std::string, std::shared_ptr<PropsFile>> trackedMapFiles_;

    /** The map of aliased files */
    std::map<std::string, std::shared_ptr<PropsFile>> aliasedMapFiles_;
};

#endif //PROPS_PROPS_FILE_TRACKER_H
