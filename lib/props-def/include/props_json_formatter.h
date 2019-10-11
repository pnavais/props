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

#ifndef PROPS_JSON_FORMATTER_H
#define PROPS_JSON_FORMATTER_H

#include <props_formatter.h>

class JsonPropsFormatter : public PropsFormatter {

    /**
     * Formats the given result in JSON format
     * appending to the given output stream.
     *
     * @param result the result
     * @param out the output stream
     */
    void format(const PropsSearchResult* result, std::ostream& out) const override;

};

#endif //PROPS_JSON_FORMATTER_H
