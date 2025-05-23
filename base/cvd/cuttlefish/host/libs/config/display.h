/*
 * Copyright (C) 2023 The Android Open Source Project
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

#include <optional>
#include <string>

#include "cuttlefish/common/libs/utils/result.h"
#include "cuttlefish/host/libs/config/cuttlefish_config.h"

namespace cuttlefish {

constexpr const char kDisplayFlag[] = "display";
constexpr const char kDisplayHelp[] =
    "Comma separated key=value pairs of display properties. Supported "
    "properties:\n"
    " 'width': required, width of the display in pixels\n"
    " 'height': required, height of the display in pixels\n"
    " 'dpi': optional, default 320, density of the display\n"
    " 'refresh_rate_hz': optional, default 60, display refresh rate in Hertz\n"
    ". Example usage: \n"
    "--display=width=1280,height=720\n"
    "--display=width=1440,height=900,dpi=480,refresh_rate_hz=30\n";

Result<std::optional<CuttlefishConfig::DisplayConfig>> ParseDisplayConfig(
    const std::string& flag);

Result<std::vector<CuttlefishConfig::DisplayConfig>>
ParseDisplayConfigsFromArgs(std::vector<std::string>& args);

}  // namespace cuttlefish
