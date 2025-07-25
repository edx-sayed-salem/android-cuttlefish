/*
 * Copyright (C) 2025 The Android Open Source Project
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

#include "cuttlefish/common/libs/utils/result.h"
#include "cuttlefish/host/libs/config/cuttlefish_config.h"
#include "cuttlefish/host/libs/image_aggregator/image_aggregator.h"

namespace cuttlefish {

class MetadataImage {
 public:
  static Result<MetadataImage> ReuseOrCreate(
      const CuttlefishConfig::InstanceSpecific&);
  static Result<MetadataImage> Reuse(const CuttlefishConfig::InstanceSpecific&);

  static std::string Name();

  ImagePartition Partition() const;

 private:
  MetadataImage(std::string);

  std::string path_;
};

}  // namespace cuttlefish
