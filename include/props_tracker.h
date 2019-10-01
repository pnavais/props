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

#ifndef PROPS_TRACKER_H
#define PROPS_TRACKER_H

#include <list>
#include "result.h"
#include "props_file.h"

class PropsTracker {

public:

    virtual ~PropsTracker() {};

    /**
     * Adds a new file to the tracker
     *
     * @param file the file to add
     */
    virtual Result add(PropsFile &file) = 0;

    /**
     * Removes a tracked file using the file name
     *
     * @param the file to remove from the tracker
     */
    virtual Result remove(const std::string &filePath)  = 0;

    /**
    * Removes a tracked file using its alias
    *
    * @param the file to remove from the tracker
    */
    virtual Result removeByAlias(const std::string &fileAlias) = 0;

    /**
     * Retrieve all tracked files
     *
     * @return the list of tracked files
     */
    virtual const std::list<PropsFile>& getTrackedFiles() const = 0;

    /**
     * Sets the file as master, revoking current
     * master condition.
     *
     * @param propsFile the file to be set as master
     */
    void updateMasterFile(PropsFile* propsFile) {
        if (masterFile_ != nullptr) {
            masterFile_->setMaster(false);
        }
        masterFile_ = propsFile;
    }

    /**
     * Retrieves the master file
     *
     * @return the master file
     */
    const PropsFile* getMasterFile() const {
        return masterFile_;
    }

    /**
     * Sets the maximum number of files to track.
     * NOTE : A negative value means no limit.
     *
     * @param maxTrackedFiles the maximum number of files to track
     */
    void setMaxTrackedFiles(const unsigned long& maxTrackedFiles) {
        maxTrackedFiles_ = maxTrackedFiles;
    }

    /**
    * Retrieves the maximum number of files to track.
    * NOTE : A negative value means no limit.
    *
    * @return the maximum number of files to track
    */
    const unsigned long& getMaxTrackedFiles() {
        return maxTrackedFiles_;
    }

    /**
     * Retrieves the list of currently tracked files
     * using the given output stream.
     */
    virtual void listTracked() const = 0;

    /**
     * Retrieves the file associated with the given alias
     * or null if not found.
     *
     * @param alias the alias
     * @return the props file or null if not found
     */
    virtual PropsFile* getFileWithAlias(const std::string& alias) = 0;

protected:

    /** The master file */
    PropsFile* masterFile_{nullptr};

    /** The maximum number of files to track (0 value means no limit) */
    unsigned long maxTrackedFiles_{0};
};

#endif //PROPS_TRACKER_H
