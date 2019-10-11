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

#ifndef PROPS_FORMATTER_H
#define PROPS_FORMATTER_H

#include "props_search_result.h"
#include <sstream>

class PropsFormatter {

public:
    PropsFormatter() = default;
    virtual ~PropsFormatter() = default;

    /**
     * Formats the given result appending
     * to the given output stream.
     *
     * @param result the result
     * @param out the output stream
     */
    virtual void format(const PropsSearchResult* result, std::ostream& out) const = 0;

};

#endif //PROPS_FORMATTER_H
