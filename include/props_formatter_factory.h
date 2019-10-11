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

#ifndef PROPS_FORMATTER_FACTORY_H
#define PROPS_FORMATTER_FACTORY_H

#include <memory>
#include <map>
#include "props_formatter.h"

namespace formatter {
    static const char DEFAULT[] = "DEFAULT";
    static const char JSON_FORMATTER[] = "JSON_FORMATTER";
}

class PropsFormatterFactory {

public:

    /**
     * Static holder for the singleton instance
     *
     * @return the singleton instance
     */
    static PropsFormatterFactory& getDefault() {
        static PropsFormatterFactory instance;
        return instance;
    }

    /**
     * Retrieves the default formatter
     *
     * @return the default formatter
     */
    static PropsFormatter* getDefaultFormatter() {
        return getDefault().defaultFormatter_;
    }

    /**
     * Retrieves the formatter with the given name or
     * null if not found.
     *
     * @param formatterName the formatter name
     * @return the formatter or null if not found
     */
    static const PropsFormatter* getFormatter(const std::string& formatterName);


private:

    PropsFormatterFactory();
    PropsFormatter* defaultFormatter_;
    std::map<std::string, std::unique_ptr<PropsFormatter>> formatterMap_;


};

#endif //PROPS_FORMATTER_FACTORY_H
