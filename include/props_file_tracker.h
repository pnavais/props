/*
 * Copyright 2018 Pablo Navais
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


class PropsFileTracker {

public:

    /**
     * Retrieves the singleton instance
     * @return the singleton instance
     */
    static PropsFileTracker& getDefault() {
        static PropsFileTracker instance;
        return instance;
    }

    /**
     * Adds a new file to the tracker
     *
     * @param file the file to add
     */
    static PropsFileTracker& add(const std::string& file) {
        getDefault().addFile(file);
        return getDefault();
    }

    /**
     * Removes a tracked file
     *
     * @param the file to remove from the tracker
     */
     static PropsFileTracker& remove(const std::string& file) {
        getDefault().removeFile(file);
        return getDefault();
     }

    /**
     * Retrieve all tracked files
     *
     * @return the list of tracked files
     */
    static const std::list<std::string>& getAll() {
        return *getDefault().trackedFiles_;
    }

    static const std::string& getMaster() {
        return getDefault().masterFile_;
    }

private:

    /**
     * Default constructor
     */
    PropsFileTracker() = default;

    /**
     * Adds the given file to the tracker
     *
     * @param file the file to add
     */
    void addFile(const std::string& file) {
        trackedFiles_->push_back(file);
    }

    /**
     * Adds the given file to the tracker
     *
     * @param file the file to add
     */
     void removeFile(const std::string &file) {
         trackedFiles_->remove(file);
     }


    /** The master file */
    std::string masterFile_;

    /** The list of tracked files */
    std::unique_ptr<std::list<std::string>> trackedFiles_;

};


#endif //PROPS_PROPS_FILE_TRACKER_H
