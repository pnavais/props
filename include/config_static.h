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

#ifndef PROPS_CONFIG_STATIC_H
#define PROPS_CONFIG_STATIC_H

#if defined(__unix__) || defined(__unix) || defined(__linux__)
#define IS_LINUX
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
#define IS_WIN
#elif defined(__APPLE__) || defined(__MACH__)
#define IS_MAC
#endif

#endif //PROPS_CONFIG_STATIC_H
