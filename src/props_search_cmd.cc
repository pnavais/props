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

#include <props_search_cmd.h>
#include <props_tracker_factory.h>
#include <exec_exception.h>
#include <sstream>

void PropsSearchCommand::parse(const int& argc, char* argv[]) {
    if (argc > 1) {
        propsTracker_ = &PropsTrackerFactory::getDefaultTracker();
        PropsCommand::parse(argc, argv);
    } else {
        throw ExecutionException("No arguments supplied");
    }

}

/**
 * Executes the track command either adding a given file
 * to the list of tracked ones or displaying the information
 * of this list.
 *
 * @param result the result message for the command
 * @return the result of the command
 */
PropsResult PropsSearchCommand::execute() {
    std::ostringstream out;
    PropsSearchResult searchResult("");

    if (optionStore_.getCmdName() == search_cmd::_SEARCH_CMD_) {
        searchResult = search();
    }

    searchResult.getExecResult().showMessage(out);
    searchResult.setOutput(out.str());
    
    return searchResult;
}

/**
 * Perform a search using the supplied options.
 *
 * @return the search results
 */
PropsSearchResult PropsSearchCommand::search() {
    std::string term;
    std::cout << "Buscando term => [" << term << "]" << std::endl;
    PropsSearchResult searchResult(term);

    return searchResult;
}