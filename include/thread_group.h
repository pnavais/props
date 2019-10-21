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

#ifndef PROPS_THREAD_GROUP_H
#define PROPS_THREAD_GROUP_H


#include <list>
#include <pthread.h>
#include <string>
#include <utility>
#include "result.h"

namespace thread_gr {
    typedef void*(thread_function)(void*);
}

class ThreadGroup {

public:

    /**
     * Creates a thread group with the given name and maximum number of threads.
     *
     * @param groupId the group identifier
     * @param maxThreads the maximum number of threads
     */
    ThreadGroup(std::string  groupId, const int& maxThreads) : maxThreads_(maxThreads), groupId_(std::move(groupId)) {}

    /**
     * Establishes the function to execute in the thread group.
     *
     * @param threadFunction the thread function
     */
    void setThreadFunction(thread_gr::thread_function* threadFunction) {
        function_ = threadFunction;
    }

    /**
     * Sets the data to be used in worker threads.
     *
     * @param data the data to be used in worker threads
     */
    void setData(void* data) {
        data_ = data;
    }

    /**
     * Retrieves the maximum number of threads to create.
     *
     * @return the maximum number of threads to create
     */
    const int& getMaxThreads() const {
        return maxThreads_;
    }

    /**
     * Creates the thread group creating the maximum
     * number of allowed threads.
     *
     * @return the result of the operation
     */
    Result start();

    /**
     * Wait for the execution of all work threads
     * in the group.
     *
     * @return the result of the operation
     */
    Result wait();

    /**
     * Wait for the execution of all work threads
     * in the group retrieving the results
     * of each thread.
     *
     * @param threadResults the results of each thread
     * @return the result of the operation
     */
    Result wait(std::list<Result>& threadResults);

    /**
     * Stops the execution of the threads in the group
     * effectively cancelling them.
     *
     * @return the result of the operation
     */
    Result stop();


private:
    int maxThreads_;
    void* data_;
    std::string groupId_;
    thread_gr::thread_function* function_;
    std::list<pthread_t> threads_;

};


#endif //PROPS_THREAD_GROUP_H
