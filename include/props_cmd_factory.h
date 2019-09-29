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

#ifndef PROPS_CMD_FACTORY_H
#define PROPS_CMD_FACTORY_H

#include "props_cmd.h"
#include <map>
#include <memory>

class PropsCommandFactory {

public:

    /**
     * Static holder for the singleton instance
     *
     * @return the singleton instance
     */
    static PropsCommandFactory& getDefault() {
        static PropsCommandFactory instance;
        return instance;
    }

    /**
     * Retrieves the command identified by the given id.
     *
     * @param id the identifier of the command
     * @return the command identified or null if not found
     */
    std::unique_ptr<PropsCommand>* getCommand(const std::string& id);

    /**
     * Retrieve all commands in the factory
     *
     * @return the list of commands
     */
    const std::list<PropsCommand *>& getCommands() const {
        return listCmds_;
    }

    /**
     * Creates a wrapper command for non a recognized command
     * argument.
     *
     * @param command the command not recognized
     * @return the wrapped command
     */
     std::unique_ptr<PropsCommand>* getUnknownCommand(const std::string& command);

private:

    PropsCommandFactory();

    std::list<PropsCommand*> listCmds_;
    std::map<std::string, std::unique_ptr<PropsCommand>> commandMap_;
};

#endif //PROPS_CMD_FACTORY_H
