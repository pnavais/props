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


#include <props_formatter_factory.h>
#include <props_simple_formatter.h>
#include <props_json_formatter.h>

/**
 * Default constructor.
 * Sets the default tracker to PropsFileTracker.
 */
PropsFormatterFactory::PropsFormatterFactory() {
    formatterMap_[formatter::DEFAULT] = std::unique_ptr<PropsFormatter>(new SimplePropsFormatter());
    formatterMap_[formatter::JSON_FORMATTER] = std::unique_ptr<PropsFormatter>(new JsonPropsFormatter());
    defaultFormatter_ = formatterMap_["DEFAULT"].get();
}

/**
 * Retrieves the formatter with the given name or
 * null if not found.
 *
 * @param formatterName the formatter name
 * @return the formatter or null if not found
 */
const PropsFormatter* PropsFormatterFactory::getFormatter(const std::string& formatterName) {
     return (getDefault().formatterMap_.count(formatterName) != 0) ? getDefault().formatterMap_.at(formatterName).get() : nullptr;
}

