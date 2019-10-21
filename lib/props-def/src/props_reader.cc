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

#include "props_reader.h"
#include "props_search_result.h"
#include "rang.hpp"
#include <fstream>
#include <sstream>
#include <pcrecpp.h>
#include <file_utils.h>
#include <props_config.h>
#include <exec_exception.h>
#include <thread_group.h>
#include <deque>

// Prototypes for globals
const pcrecpp::RE &COMMENTED_LINE();
void* search_files(void* data);

/**
 * Namespace for reader
 */
namespace reader {
    static const long DEFAULT_MAX_WORKER_THREADS = 5;
    static const char* MAX_WORKER_THREADS = "general.max_worker_threads";
}

// Controls the file queue access
pthread_mutex_t filesQueueMutex;

/**
 * Retrieves the regular expression for
 * commented lines.
 *
 * @return the regex for commented lines
 */
const pcrecpp::RE &COMMENTED_LINE() {
    static const pcrecpp::RE COMMENTED_LINE("^#");
    return COMMENTED_LINE;
}

/**
 * Process a file queue finding potential matches for
 * the search terms provided.
 * 
 * @param data the files queue and search terms
 * @return the result of the operation
 */
void* search_files(void* data) {
    auto* result = new Result{res::VALID};
    bool keep_processing = true;

    if (data != nullptr) {
        auto *searchData = (search::FileSearchData*) data;
        auto *filesQueue = searchData->filesQueue_;
        auto* regex = (pcrecpp::RE*)searchData->regex_;

        while (keep_processing) {
            pthread_mutex_lock (&filesQueueMutex);

            std::unique_ptr<PropsFile> file;

            if (!filesQueue->empty()) {
                file.reset(new PropsFile(filesQueue->back()));
                filesQueue->pop_back();
            } else {
                keep_processing = false;
            }

            pthread_mutex_unlock(&filesQueueMutex);

            if (file != nullptr) {
                const std::string &input = searchData->searchOptions_->key_;
                pcrecpp::StringPiece value_k;
                pcrecpp::StringPiece value_r;

                const std::string &fullPath = FileUtils::getAbsolutePath(file->getFileName());
                std::ifstream infile(fullPath);

                if (infile) {
                    // TODO: Process dividing file in chunks and in parallel (mmap ?)
                    std::string line;
                    while (std::getline(infile, line)) {
                        // Try to find the regex in line, and keep results.
                        if (!COMMENTED_LINE().PartialMatch(line)) {
                            std::cout << "Entro con line " << std::endl;
                            if (regex->PartialMatch(line, &value_k, &value_r)) {
                                size_t pos_k = value_k.data() - line.data();
                                size_t pos_r = value_r.data() - line.data();
                                std::cout << "HAY MATCHINGER Z !!!" << std::endl;
                                searchData->searchResult_->add(file->getFileName(), p_search_res::Match{input,
                                                                                    *(searchData->searchOptions_),
                                                                                    line,
                                                                                    p_search_res::StringMatch{
                                                                                            value_k.as_string(), pos_k},
                                                                                    p_search_res::StringMatch{
                                                                                            value_r.as_string(),
                                                                                            pos_r}});
                            }
                        }
                    }
                    infile.close();
                } else {
                    std::cerr << rang::fgB::red << "File \"" << file->getFileName() << "\" not found" << rang::fg::reset
                              << std::endl;
                }
            }            
        }
    }

    pthread_exit(result);
}

/**
 * Finds the value for the key in the specified file.
 *
 * @param key the key to search
 * @param file the source file
 * @return the value for the key in the file
 */
std::unique_ptr<PropsSearchResult> PropsReader::find_value(p_search_res::SearchOptions &searchOptions, const std::list<PropsFile> &files) {
    search::FileSearchData fileSearchData = buildSearchData(searchOptions, files);
    std::unique_ptr<PropsSearchResult> searchResult(fileSearchData.searchResult_);

    // Configure threading
    auto maxWorkerThreads = PropsConfig::getDefault().getValue<size_t>(reader::MAX_WORKER_THREADS, reader::DEFAULT_MAX_WORKER_THREADS);
    maxWorkerThreads = (maxWorkerThreads > files.size()) ? files.size() : maxWorkerThreads;

    pthread_mutex_init(&filesQueueMutex, nullptr);

    ThreadGroup threadGroup("READER_GROUP", maxWorkerThreads);
    threadGroup.setThreadFunction(search_files);
    threadGroup.setData(&fileSearchData);
    threadGroup.start();
    threadGroup.wait();

    pthread_mutex_destroy(&filesQueueMutex);

	return searchResult;
}

/**
 *  Finds the value of the given key searching in the mater tracked file
 *  or globally on every tracked file and replace its value with the given
 *  new one.
 *
 * @param key the key to find
 * @param newValue the new value to set
 * @param files the list of files to search
 *
 * @return the results of the search
 */
std::unique_ptr<PropsSearchResult> PropsReader::find_and_replace_value(p_search_res::SearchOptions& searchOptions, const std::string& newValue, const std::list<PropsFile>& files) {
    std::cout << "NEW VALUE => [" << newValue << "]" << std::endl;
    search::FileSearchData fileSearchData = buildSearchData(searchOptions, files);
    std::unique_ptr<PropsSearchResult> searchResult(fileSearchData.searchResult_);

    // Configure threading
    auto maxWorkerThreads = PropsConfig::getDefault().getValue<size_t>(reader::MAX_WORKER_THREADS, reader::DEFAULT_MAX_WORKER_THREADS);
    maxWorkerThreads = (maxWorkerThreads > files.size()) ? files.size() : maxWorkerThreads;

    pthread_mutex_init(&filesQueueMutex, nullptr);

    ThreadGroup threadGroup("READER_GROUP", maxWorkerThreads);
    threadGroup.setThreadFunction(search_files);
    threadGroup.setData(&fileSearchData);
    threadGroup.start();
    threadGroup.wait();

    pthread_mutex_destroy(&filesQueueMutex);

    return searchResult;
}

/**
 * Amend input options if default values needed.
 *
 * @param searchOptions the search options
 */
void PropsReader::fixSearchOptions(p_search_res::SearchOptions &searchOptions) {

    // Amend search options by retrieving default values if needed
    if (searchOptions.caseSensitive_ == p_search_res::DEFAULT) {
        bool ignore_case = PropsConfig::getDefault().getValue<bool>(search::KEY_IGNORE_CASE, search::DEFAULT_IGNORE_CASE);
        searchOptions.caseSensitive_ = (ignore_case) ? p_search_res::NO_OPT : p_search_res::USE_OPT;
    }

    if (searchOptions.partialMatch_ == p_search_res::DEFAULT) {
        bool allow_partial_match = PropsConfig::getDefault().getValue<bool>(search::KEY_ALLOW_PARTIAL_MATCH, search::DEFAULT_ALLOW_PARTIAL_MATCH);
        searchOptions.partialMatch_ = (allow_partial_match) ? p_search_res::USE_OPT : p_search_res::NO_OPT;
    }

    if (searchOptions.separator_.empty()) {
        searchOptions.separator_ = PropsConfig::getDefault().getValue<std::string>(search::KEY_SEPARATOR, search::DEFAULT_KEY_SEPARATOR);
    }
}

/**
 * Builds the search regular expression from the given search options.
 *
 * @param searchOptions the search options
 * @param regex_str the built string regex
 */
void PropsReader::buildRegex(const p_search_res::SearchOptions& searchOptions, std::string& regex_str) {
    std::string key   = searchOptions.key_;
    const bool& matchValue   = searchOptions.matchValue_;

    if (!searchOptions.isRegex_) {
        key = pcrecpp::RE::QuoteMeta(key);
    }

    // Prepare regex
    std::stringstream regex_stream;
    std::string partStr = (searchOptions.partialMatch_ == p_search_res::USE_OPT) ? ".*?" : "";

    if (matchValue) {
        regex_stream << "^(.+)" << searchOptions.separator_  << partStr << "(" << key  << ")" << partStr << "$";
    } else {
        if (key.back() == '$') {
            std::string key_rpl = key.substr(0, key.length() - 1);
            regex_stream << "^" << partStr << "(" << key_rpl << ")" << searchOptions.separator_  << "(.+)";
        } else {
            regex_stream << "^" << partStr << "(" << key << ")" << partStr << searchOptions.separator_  << "(.+)";
        }
    }

    regex_str = regex_stream.str();
}

/**
 * Retrieves the search data for the given options and file list.
 *
 * @param searchOptions the search options
 * @param files the list of files to process
 * @return the built search data
 */
search::FileSearchData PropsReader::buildSearchData(p_search_res::SearchOptions& searchOptions, const std::list<PropsFile>& files) {

    auto* pSearchResult = new PropsSearchResult(searchOptions);

    // Amend options if defaults needed
    fixSearchOptions(searchOptions);

    // Regex options
    pcrecpp::RE_Options opt;
    opt.set_caseless((searchOptions.caseSensitive_ == p_search_res::NO_OPT));

    // Build regex
    std::string regex_in;
    buildRegex(searchOptions, regex_in);
    pcrecpp::RE regex(regex_in, opt);

    if (regex.NumberOfCapturingGroups() > 2) {
        throw ExecutionException("Too many capture groups specified");
    }

    // Fill the queue with input files
    auto* pFilesQueue = new std::deque<PropsFile>();
    for (auto& file : files) {
        pFilesQueue->push_back(file);
    }

    return search::FileSearchData { &searchOptions, &regex, pFilesQueue, pSearchResult };
}
