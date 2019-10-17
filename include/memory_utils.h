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

#ifndef PROPS_MEMORY_UTILS_H
#define PROPS_MEMORY_UTILS_H

class MemoryUtils {

public:

    MemoryUtils()  = delete;
    ~MemoryUtils() = delete;

    /**
     * Free the memory used by a two dimensional array.
     *
     * @tparam T the type
     * @param list the 2D array
     * @param size the size of the array
     */
    template<typename T>
    static void free2DArray(T** list, int size) {
        if (list != nullptr) {
            for (int i=0; i<size; i++) {
                delete[] list[i];
            }
            delete[] list;
        }
    }

};

#endif //PROPS_MEMORY_UTILS_H
