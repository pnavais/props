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

#include "thread_group.h"

/**
 * Creates the thread group creating the maximum
 * number of allowed threads.
 *
 * @return the result of the operation
 */
Result ThreadGroup::start() {
    Result result{res::VALID};
    int rc;

    // Initialize and set thread detached attribute. No all pthread implementations
    // creates joinable threads by default. Explicitly stating it
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for (int i=0; i<maxThreads_; i++) {
        pthread_t t;
        rc = pthread_create(&t, nullptr, function_, data_);
        if (rc) {
            result = res::ERROR;
            result.setSeverity(res::CRITICAL);
            result.setMessage("Error creating thread ["+std::to_string(i+1)+"/"+std::to_string(maxThreads_)+"]. Return code : "+std::to_string(rc));
            break;
        }

        threads_.push_back(t);
    }

    pthread_attr_destroy(&attr);

    return result;
}

/**
 * Wait for the execution of all work threads
 * in the group.
 *
 * @return the result of the operation
 */
Result ThreadGroup::wait() {
    Result result{res::VALID};
    void *status = nullptr;
    int rc;
    int i=1;
    for (auto& t : threads_) {
        rc = pthread_join(t, &status);
        if (status != nullptr) {
            delete (Result*)status;
        }
        if (rc) {
            result = res::ERROR;
            result.setSeverity(res::CRITICAL);
            result.setMessage("Error waiting for thread ["+std::to_string(i)+"/"+std::to_string(maxThreads_)+"]. Return code : "+std::to_string(rc));
            break;
        }
        i++;
    }

    return result;
}

/**
 * Wait for the execution of all work threads
 * in the group retrieving the results
 * of each thread.
 *
 * @param threadResults the results of each thread
 * @return the result of the operation
 */
Result ThreadGroup::wait(std::list<Result>& threadResults) {
    Result result{res::VALID};
    void *status = nullptr;
    int rc;
    int i=1;
    for (auto& t : threads_) {
        rc = pthread_join(t, &status);
        if (status != nullptr) {
            auto* pRes = (Result*) status;
            threadResults.emplace_back(*pRes);
            delete (Result*)status;
        }
        if (rc) {
            result = res::ERROR;
            result.setSeverity(res::CRITICAL);
            result.setMessage("Error waiting for thread ["+std::to_string(i)+"/"+std::to_string(maxThreads_)+"]. Return code : "+std::to_string(rc));
            break;
        }
        i++;
    }

    return result;
}

/**
 * Stops the execution of the threads in the group
 * effectively cancelling them.
 *
 * @return the result of the operation
 */
Result ThreadGroup::stop() {
    Result result{res::VALID};
    int rc;

    int i=1;
    for (auto& t : threads_) {
        rc = pthread_cancel(t);
        if (rc) {
            result = res::ERROR;
            result.setSeverity(res::CRITICAL);
            result.setMessage("Error stopping thread ["+std::to_string(i)+"/"+std::to_string(maxThreads_)+"]. Return code : "+std::to_string(rc));
            break;
        }
        threads_.remove(t);
        i++;
    }

    return result;
}