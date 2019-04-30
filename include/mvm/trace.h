// Copyright 2019 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <iostream>
#include <sstream>

#ifdef ENABLE_TRACES
#define LOG_INFO(ss) std::cout << "[MVM][INFO] " << ss << std::endl
#define LOG_ERROR(ss) std::cerr << "[MVM][ERROR] " << ss << std::endl
#else
#define LOG_INFO(ss)
#define LOG_ERROR(ss)
#endif // ENABLE_TRACES
