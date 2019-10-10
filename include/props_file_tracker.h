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

#ifndef PROPS_FILE_TRACKER_H
#define PROPS_FILE_TRACKER_H

#include <list>
#include <iostream>
#include <memory>
#include <algorithm>
#include <map>
#include "result.h"
#include "props_file.h"
#include "props_tracker.h"

class PropsFileTracker : public PropsTracker {


public:

    /**
     * Default constructor
     */
    PropsFileTracker();

    /**
     * Adds a new file to the tracker
     *
     * @param file the file to add
     */
    Result add(PropsFile &file) override {
        Result result{res::VALID};
        addFile(file, result);
        return result;
    }

    /**
     * Removes a tracked file using the file name
     *
     * @param the file to remove from the tracker
     */
    Result remove(const std::string &filePath) override {
        Result result{res::VALID};
        removeFile(filePath, result);
        return result;
    }

    /**
     * Removes a tracked file using its alias
     *
     * @param the file to remove from the tracker
     */
    Result removeByAlias(const std::string &fileAlias) override {
        Result result{res::VALID};
        removeFileByAlias(fileAlias, result);
        return result;
    }

    /**
     * Sets the alias for a given file
     *
     * @param fileName the file to alias
     * @param fileAlias the alias to be set
     * @return the result of the operation
     */
    Result setAlias(const std::string& fileName, const std::string &fileAlias) override;

    /**
     * Removes the given alias from the file.
     *
     * @param alias alias to remove
     */
    Result removeAlias(const std::string& alias) override;

    /**
     * Removes the current alias from the file.
     *
     * @param fileName the file to remove the alias from
     * @return the result of the operation
     */
    Result removeFileAlias(const std::string& fileName) override;

    /**
     * Retrieve all tracked files
     *
     * @return the list of tracked files
     */
    const std::list<PropsFile>& getTrackedFiles() const override {
        return trackedFiles_;
    }

    /**
     * Stores the current configuration of the tracker
     *
     * @return the result of the operation
     */
    Result save() override;

    /**
     * Removes all currently tracked files
     *
     * @return the result of the operation
     */
    Result clear() override;

    /**
     * Retrieves the list of currently tracked files
     * using the given output stream.
     */
    void listTracked() const override {
        listTracked(std::cout);
    };

    /**
     * Retrieves the list of currently tracked files
     * using the given output stream.
     */
    void listTracked(std::ostream &output) const;

    /**
     * Retrieves the file associated with the given alias
     * or null if not found.
     *
     * @param alias the alias
     * @return the props file or null if not found
     */
    PropsFile* getFileWithAlias(const std::string& alias) override;

    /**
     * Retrieves the file with the given name
     * or null if not found.
     *
     * @param file the file name
     * @return the props file or null if not found
     */
    PropsFile* getFile(const std::string& file) override;

    /**
     * Retrieves the files associated with a given
     * group.
     *
     * @param group the group name
     * @return the list of files of the group
     */
    const std::list<PropsFile*>* getGroup(const std::string& group) override {
        std::list<PropsFile *> *groupFiles = nullptr;
        if (trackedGroups_.count(group) != 0) {
            groupFiles = &trackedGroups_.at(group);
        }
        return groupFiles;
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
    Result group(const std::string& name, const std::string& targetGroup, const bool& isAlias) override;

    /**
     * Removes the group effectively moving all its
     * contained files to the default group.
     *
     * @param group the group to remove
     * @return the result of the operation
     */
    Result removeGroup(const std::string& group) override;

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
    Result renameGroup(const std::string& sourceGroup, const std::string& targetGroup, const bool& force) override;

private:

    /**
     * Parses the tracker current configuration from the default
     * user's config file.
     */
    void parseTrackerConfig();


    /**
     * Sets the given file as master.
     *
     * @param propsFile the file to be set as master
     */
    void setMasterFile(PropsFile* propsFile) {
        masterFile_ = propsFile;
    }

    /**
     * Sets the front of the tracked files list (if any)
     * as master.
     */
    void setFirstAsMaster();

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
     * Updates or creates if needed, the tracker config file with the current
     * tracked configuration.
     */
    void updateTrackerConfig() const;

    /**
     * Writes the tracker current configuration to the given output file.
     *
     * @param outputFilePath the path to the output file
     */
    void writeTrackerConfig(const std::string& outputFilePath) const;

    /**
     * Removes the given file from its groups (if any).
     *
     * @param propsFile the props file
     */
    void removeFileFromGroup(PropsFile* propsFile);

    /**
     * Print the tracked group.
     *
     * @param output the output stream
     * @param maxFileNameSize  the maximum file name size
     * @param groupName the group name
     */
    void printTrackedGroup(std::ostream &output, size_t maxFileNameSize, const std::string& groupName) const;

    /** The list of tracked files */
    std::list<PropsFile> trackedFiles_;

    /** The map of tracked files */
    std::map<std::string,PropsFile*> trackedMapFiles_;

    /** The map of aliased files */
    std::map<std::string, PropsFile*> aliasedMapFiles_;

    /** The groups of tracked files */
    std::map<std::string, std::list<PropsFile*>> trackedGroups_;

};

#endif //PROPS_FILE_TRACKER_H
