/*
 * Copyright (C) 2021 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <unordered_map>

#include "cuttlefish/common/libs/utils/result.h"
#include "cuttlefish/host/libs/config/cuttlefish_config.h"

namespace cuttlefish {

Result<std::unordered_map<std::string, std::string>> BootconfigArgsFromConfig(
    const CuttlefishConfig& config,
    const CuttlefishConfig::InstanceSpecific& instance);

Result<std::string> BootconfigArgsString(
    const std::unordered_map<std::string, std::string>& args,
    const std::string& separator);

}  // namespace cuttlefish
