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

#include <props_edit_cmd.h>
#include <props_tracker_factory.h>
#include <exec_exception.h>
#include <sstream>
#include <props_reader.h>

void PropsEditCommand::parse(const int& argc, char* argv[]) {

    if (argc > 1) {
        propsTracker_ = &PropsTrackerFactory::getDefaultTracker();
        PropsCommand::parse(argc, argv);
    } else {
        throw ExecutionException("No arguments supplied");
    }

    int searchOptions = 0;

    // Check only one search option has been supplied
    if (optionStore_.getOptions().count(edit_cmd::_ALIAS_FILE_) != 0) {
        searchOptions++;
    }

    if (optionStore_.getOptions().count(edit_cmd::_GROUP_SEARCH_) != 0) {
        searchOptions++;
    }

    if (optionStore_.getOptions().count(edit_cmd::_MULTI_SEARCH_) != 0) {
        searchOptions++;
    }

    // Check that only files or search options are supplied
    if (optionStore_.getArgs().size() > 2 && searchOptions > 0) {
        throw ExecutionException("Only one search option allowed [Files or Tracker]");
    }

    if (searchOptions > 1) {
        throw ExecutionException("Only one search option allowed [Alias, Group, Multi]");
    }
}

/**
 * Executes the edit command to perform the modification
 * of matching keys.
 *
 * @param result the result message for the command
 * @return the result of the command
 */
std::unique_ptr<PropsResult> PropsEditCommand::execute() {
    std::ostringstream out;
    std::unique_ptr<PropsResult> searchResult = nullptr;

    if (optionStore_.getCmdName() == edit_cmd::_EDIT_CMD_) {
        searchResult = modify();
    }

    if (searchResult != nullptr) {
        searchResult->getExecResult().showMessage(out);
        searchResult->setOutput(out.str());
    } else {
        searchResult.reset(new PropsResult());
    }

    return searchResult;
}

/**
 * Perform a search using the supplied options.
 *
 * @return the modification results
 */
std::unique_ptr<PropsResult> PropsEditCommand::modify() {
    Result res{res::VALID};
    auto it = std::begin(optionStore_.getArgs());
    const std::string& term  = *it;
    const std::string& value = *++it;
    std::unique_ptr<PropsSearchResult> searchResult(nullptr);

    // Retrieve search options
    std::string keySeparator = (optionStore_.getOptions().count(edit_cmd::_SEPARATOR_) != 0) ?
                                optionStore_.getOptions().at(edit_cmd::_SEPARATOR_) : "";

    p_search_res::Opt caseSensitive     = (optionStore_.getOptions().count(edit_cmd::_IGNORE_CASE_) != 0)   ? p_search_res::NO_OPT : p_search_res::DEFAULT;
    p_search_res::Opt allowPartialMatch = (optionStore_.getOptions().count(edit_cmd::_PARTIAL_MATCH_) != 0) ? p_search_res::USE_OPT : p_search_res::DEFAULT;
    const bool& matchValue              = false;
    const bool& isRegex                 = (optionStore_.getOptions().count(edit_cmd::_USE_REGEX_) != 0);

    // Compute the list of input files
    std::list<PropsFile> fileList;
    retrieveFileList(fileList, res);

    p_search_res::SearchOptions searchOptions{term, caseSensitive, keySeparator, allowPartialMatch, matchValue, isRegex};

    if (fileList.empty()) {
        res = res::ERROR;
        res.setSeverity(res::WARN);
        res.setMessage("There are no files to lookup");
        searchResult.reset(new PropsSearchResult(searchOptions));
        searchResult->setResult(res);
    } else {
        searchResult = PropsReader::find_and_replace_value(searchOptions, value, fileList);
        searchResult->setResult(res);
    }

    return searchResult;
}

/**
 * Retrieves the list of files to lookup from the given options.
 *
 * @param fileList the list of files
 * @param res the output result in case of errors.
 */
void PropsEditCommand::retrieveFileList(std::list<PropsFile>& fileList, Result& res) {
    // Check if files supplied manually
    if (optionStore_.getArgs().size() > 2) {
        // Skip key and value arguments and consider the rest as files
        auto it = std::begin(optionStore_.getArgs());
        std::advance(it, 2);
        for (auto end = std::end(optionStore_.getArgs()); it != end; ++it) {
            fileList.push_back(PropsFile::make_file(*it));
        }
    } else {
        const auto& option_map = optionStore_.getOptions();
        // Check an aliased file was supplied
        if (option_map.count(edit_cmd::_ALIAS_FILE_) != 0) {
            auto& alias = option_map.at(edit_cmd::_ALIAS_FILE_);
            auto* pFile = propsTracker_->getFileWithAlias(alias);
            if (pFile != nullptr) {
                fileList.push_back(*pFile);
            } else {
                res = res::ERROR;
                res.setMessage("Alias \"" + alias + "\" not found");
            }
        } else if (option_map.count(edit_cmd::_MULTI_SEARCH_) != 0) {
            fileList = propsTracker_->getTrackedFiles();
        } else if (option_map.count(edit_cmd::_GROUP_SEARCH_) != 0) {
            auto& group = option_map.at(edit_cmd::_GROUP_SEARCH_);
            const std::list<PropsFile *> *pGroupList = propsTracker_->getGroup(group);
            if (pGroupList != nullptr) {
                for (auto pFile : *pGroupList) {
                    fileList.push_back(*pFile);
                }
            } else {
                res = res::ERROR;
                res.setMessage("Group \"" + group + "\" not found");
            }
        } else {
            // Take master if no other option set
            auto masterFile = propsTracker_->getMasterFile();
            if (masterFile != nullptr) {
                fileList.push_back(*masterFile);
            }
        }
    }
}