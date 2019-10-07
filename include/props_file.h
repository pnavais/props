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

#ifndef PROPS_FILE_H
#define PROPS_FILE_H

#include <string>

/**
 * A simple holder for a file handled by Props.
 * Currently contains :
 * - The master flag: If set the file is used as the default one
 *                    for read/write operations
 * - The file name  : The full path to the file being tracked
 * - The alias      : The alias for the file
 */
class PropsFile {

public:

    /**
     * Creates a new properties file identified by the given
     * file name.
     *
     * @param fileName the file name
     * @return the file
     */
    static PropsFile make_file(const std::string& fileName) {
        PropsFile propsFile;
        propsFile.setFileName(fileName);
        return propsFile;
    }

    /**
     * Retrieves whether the file has been
     * set as master or not.
     *
     * @return the master flag
     */
    bool isMaster() const {
        return master_;
    }

    /**
     * Sets the file as master or not.
     *
     * @param master the master flag
     */
    void setMaster(bool master) {
        master_ = master;
    }

    /**
     * Retrieves the file name.
     *
     * @return the file name
     */
    const std::string &getFileName() const {
        return fileName_;
    }

    /**
     * Sets the file name.
     *
     * @param fileName the file name to set
     */
    void setFileName(const std::string &fileName) {
        fileName_ = fileName;
    }

    /**
     * Retrieves the alias for the file.
     *
     * @return the file alias
     */
    const std::string &getAlias() const {
        return alias_;
    }

    /**
     * Sets the alias for the file.
     *
     * @param alias the file alias
     */
    void setAlias(const std::string &alias) {
        alias_ = alias;
    }

    /**
     * Retrieves the group for the file.
     *
     * @return the file group
     */
    const std::string& getGroup() const {
        return group_;
    }

    /**
     * Sets the group for the file.
     *
     * @param group the group name
     */
    void setGroup(const std::string& group) {
        group_ = group;
    }

    /**
    * Equality operator.
    *
    * @param rhs the other props file
    * @return true if equals, false otherwise
    */
    bool operator==(const PropsFile &rhs) const {
        return fileName_ == rhs.fileName_;
    }

    /**
* Equality operator.
*
* @param rhs the other props file
* @return true if equals, false otherwise
*/
    bool operator==(const std::string &filePath) const {
        return fileName_ == filePath;
    }

    /**
     * Distinct operator.
     *
     * @param rhs the other props file
     * @return true if different, false otherwise
     */
    bool operator!=(const PropsFile &rhs) const {
        return !(rhs == *this);
    }

private:
    bool master_ = false;
    std::string fileName_;
    std::string alias_;
    std::string group_;

};

#endif //PROPS_FILE_H
