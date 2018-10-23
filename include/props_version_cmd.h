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

#ifndef PROPS_PROPS_VERSION_COMMAND_H
#define PROPS_PROPS_VERSION_COMMAND_H

#include "config.h"
#include "props_cmd.h"
#include <sstream>

class PropsVersionCommand : public PropsCommand {

public:

    PropsVersionCommand() {
        id_          = "version";
        name_        = "-v|--version";
        tagLine_     = "Display current version and exit";
        description_ = tagLine_;
    }

    void execute(PropsResult &result) override {
        std::ostringstream out;
        out << PACKAGE_NAME << " version " << PACKAGE_VERSION << std::endl;
        result.setResult_(out.str());
    }

};

#endif //PROPS_PROPS_VERSION_COMMAND_H
