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

namespace tracker {
    static const char DEFAULT_GROUP[] = "_default";
}

class PropsTracker {

public:

    virtual ~PropsTracker() = default;

    /**
     * Adds a new file to the tracker
     *
     * @param file the file to add
     */
    virtual Result add(PropsFile &file) = 0;

    /**
     * Adds new files to the tracker
     *
     * @param file the file to add
     */
    virtual Result add(std::list<PropsFile>& files) = 0;

    /**
     * Removes a tracked file using the file name
     *
     * @param filePath the file to remove from the tracker
     */
    virtual Result remove(const std::string &filePath)  = 0;

    /**
     * Removes a tracked file using its alias
     *
     * @param fileAlias the file to remove from the tracker
     * @return the result of the operation
     */
    virtual Result removeByAlias(const std::string &fileAlias) = 0;

    /**
     * Sets the alias for a given file
     *
     * @param fileName the file to alias
     * @param fileAlias the alias to be set
     * @return the result of the operation
     */
    virtual Result setAlias(const std::string& fileName, const std::string &fileAlias) = 0;

    /**
     * Removes the given alias from the file.
     *
     * @param alias alias to remove
     * @return the result of the operation
     */
    virtual Result removeAlias(const std::string& alias) = 0;

    /**
     * Removes the current alias from the file.
     *
     * @param fileName the file to remove the alias from
     * @return the result of the operation
     */
    virtual Result removeFileAlias(const std::string& fileName) = 0;

    /**
     * Retrieve all tracked files
     *
     * @return the list of tracked files
     */
    virtual const std::list<PropsFile>& getTrackedFiles() const = 0;

    /**
     * Stores the current configuration of the tracker
     *
     * @return the result of the operation
     */
    virtual Result save() = 0;

    /**
     * Removes all currently tracked files
     *
     * @return the result of the operation
     */
    virtual Result clear() = 0;

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
        if (propsFile != nullptr) {
            propsFile->setMaster(true);
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

    /**
     * Retrieves the file with the given name
     * or null if not found.
     *
     * @param file the file name
     * @return the props file or null if not found
     */
    virtual PropsFile* getFile(const std::string& file) = 0;

    /**
     * Retrieves the files associated with a given
     * group.
     *
     * @param group the group name
     * @return the list of files of the group
     */
    virtual const std::list<PropsFile*>* getGroup(const std::string& group) = 0;

    /**
     * Moves the object specified by its file/alias
     * to the target group.
     *
     * @param name the file name or alias
     * @param targetGroup the target group
     * @param isAlias true if the name is an alias
     * @return the result of the operation
     */
    virtual Result group(const std::string& name, const std::string& targetGroup, const bool& isAlias) = 0;

    /**
     * Removes the group effectively moving all its
     * contained files to the default group.
     *
     * @param group the group to remove
     * @param untrack removes the files instead of moving to default group
     * @return the result of the operation
     */
    virtual Result removeGroup(const std::string& group, const bool& untrack = false) = 0;

    /**
     * Moves the files from source group to target group.
     * If target group exists, the rename operation only is
     * effected if the force flag is supplied.
     *
     * @param sourceGroup the source group
     * @param targetGroup the target group
     * @param force forces the operation if target group exists
     * @return the result of the operation
     */
    virtual Result renameGroup(const std::string& sourceGroup, const std::string& targetGroup, const bool& force) = 0;

protected:

    /** The master file */
    PropsFile* masterFile_{nullptr};

    /** The maximum number of files to track (0 value means no limit) */
    unsigned long maxTrackedFiles_{0};
};

#endif //PROPS_TRACKER_H
