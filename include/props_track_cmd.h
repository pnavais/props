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

#ifndef PROPS_PROPS_TRACK_COMMAND_H
#define PROPS_PROPS_TRACK_COMMAND_H

#include "config.h"
#include "props_cmd.h"
#include <map>
#include <sstream>

namespace track_cmd {
    static const char* _TRACKED_FILE_ = "_TRACKED_FILE_ID_";
}

class PropsTrackCommand : public PropsCommand {

public:

    PropsTrackCommand() {
        id_ = name_  = "track";
        tagLine_     = "Add | List tracked files";
        description_ = "Adds a properties file to the list of tracked files stored in $HOME/.config/props.track. In case the file does "
                       "not exist a warning message is displayed and the file is ignored.Once added to the list of tracked files "
                       ", the file will be queried in case global option is active.In case the tracked file is flagged as master, "
                       "the file would be used as main source in case global option is disabled."
                       "Additionally this command allows listing all "
                       "currently tracked properties files.";
        args_ = { PropsArg::make_arg("file" , "File to track", { PropsArg::make_arg("-m", "Set file as master"), PropsArg::make_arg("-lapolla", "La polla records") }),
                  PropsArg::make_arg("ls"   , "List all tracked files") };
    }

    void parse(const int &argc, const char **argv) override;

    void execute(PropsResult &result) override;

private:

    std::map<std::string, std::string> options_;

};

#endif //PROPS_PROPS_TRACK_COMMAND_H
