//
// Copyright (C) 2022 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "cuttlefish/host/commands/assemble_cvd/disk_builder.h"

#include <sstream>
#include <string>
#include <vector>

#include <android-base/file.h>

#include "cuttlefish/common/libs/utils/files.h"
#include "cuttlefish/host/libs/config/cuttlefish_config.h"
#include "cuttlefish/host/libs/image_aggregator/image_aggregator.h"
#include "cuttlefish/host/libs/image_aggregator/qcow2.h"

namespace cuttlefish {

static std::chrono::system_clock::time_point LastUpdatedInputDisk(
    const std::vector<ImagePartition>& partitions) {
  std::chrono::system_clock::time_point ret;
  for (auto& partition : partitions) {
    if (partition.label == "frp") {
      continue;
    }

    auto partition_mod_time =
        FileModificationTime(partition.image_file_path)
            .value_or(std::chrono::system_clock::time_point());
    if (partition_mod_time > ret) {
      ret = partition_mod_time;
    }
  }
  return ret;
}

DiskBuilder& DiskBuilder::EntireDisk(std::string path) & {
  entire_disk_ = std::move(path);
  return *this;
}
DiskBuilder DiskBuilder::EntireDisk(std::string path) && {
  entire_disk_ = std::move(path);
  return *this;
}

DiskBuilder& DiskBuilder::Partitions(std::vector<ImagePartition> partitions) & {
  partitions_ = std::move(partitions);
  return *this;
}
DiskBuilder DiskBuilder::Partitions(std::vector<ImagePartition> partitions) && {
  partitions_ = std::move(partitions);
  return *this;
}

DiskBuilder& DiskBuilder::HeaderPath(std::string header_path) & {
  header_path_ = std::move(header_path);
  return *this;
}
DiskBuilder DiskBuilder::HeaderPath(std::string header_path) && {
  header_path_ = std::move(header_path);
  return *this;
}

DiskBuilder& DiskBuilder::FooterPath(std::string footer_path) & {
  footer_path_ = std::move(footer_path);
  return *this;
}
DiskBuilder DiskBuilder::FooterPath(std::string footer_path) && {
  footer_path_ = std::move(footer_path);
  return *this;
}

DiskBuilder& DiskBuilder::CrosvmPath(std::string crosvm_path) & {
  crosvm_path_ = std::move(crosvm_path);
  return *this;
}
DiskBuilder DiskBuilder::CrosvmPath(std::string crosvm_path) && {
  crosvm_path_ = std::move(crosvm_path);
  return *this;
}

DiskBuilder& DiskBuilder::VmManager(VmmMode vm_manager) & {
  vm_manager_ = std::move(vm_manager);
  return *this;
}
DiskBuilder DiskBuilder::VmManager(VmmMode vm_manager) && {
  vm_manager_ = std::move(vm_manager);
  return *this;
}

DiskBuilder& DiskBuilder::ConfigPath(std::string config_path) & {
  config_path_ = std::move(config_path);
  return *this;
}
DiskBuilder DiskBuilder::ConfigPath(std::string config_path) && {
  config_path_ = std::move(config_path);
  return *this;
}

DiskBuilder& DiskBuilder::CompositeDiskPath(std::string composite_disk_path) & {
  composite_disk_path_ = std::move(composite_disk_path);
  return *this;
}
DiskBuilder DiskBuilder::CompositeDiskPath(std::string composite_disk_path) && {
  composite_disk_path_ = std::move(composite_disk_path);
  return *this;
}

DiskBuilder& DiskBuilder::OverlayPath(std::string overlay_path) & {
  overlay_path_ = std::move(overlay_path);
  return *this;
}
DiskBuilder DiskBuilder::OverlayPath(std::string overlay_path) && {
  overlay_path_ = std::move(overlay_path);
  return *this;
}

DiskBuilder& DiskBuilder::ResumeIfPossible(bool resume_if_possible) & {
  resume_if_possible_ = resume_if_possible;
  return *this;
}
DiskBuilder DiskBuilder::ResumeIfPossible(bool resume_if_possible) && {
  resume_if_possible_ = resume_if_possible;
  return *this;
}

DiskBuilder& DiskBuilder::ReadOnly(bool read_only) & {
  read_only_ = read_only;
  return *this;
}
DiskBuilder DiskBuilder::ReadOnly(bool read_only) && {
  read_only_ = read_only;
  return *this;
}

Result<std::string> DiskBuilder::TextConfig() {
  std::ostringstream disk_conf;

  CF_EXPECT(vm_manager_ != VmmMode::kUnknown, "Missing vm_manager");
  disk_conf << vm_manager_ << "\n";

  CF_EXPECT(!partitions_.empty() ^ !entire_disk_.empty(),
            "Specify either partitions or a whole disk");
  for (auto& partition : partitions_) {
    disk_conf << partition.image_file_path << "\n";
  }
  if (!entire_disk_.empty()) {
    disk_conf << entire_disk_;
  }
  return disk_conf.str();
}

Result<bool> DiskBuilder::WillRebuildCompositeDisk() {
  if (!resume_if_possible_) {
    return true;
  }

  CF_EXPECT(!config_path_.empty(), "No config path");
  if (ReadFile(config_path_) != CF_EXPECT(TextConfig())) {
    LOG(DEBUG) << "Composite disk text config mismatch";
    return true;
  }

  CF_EXPECT(!partitions_.empty() ^ !entire_disk_.empty(),
            "Specify either partitions or a whole disk");
  if (!entire_disk_.empty()) {
    LOG(DEBUG) << "No composite disk to build";
    return false;
  }
  auto last_component_mod_time = LastUpdatedInputDisk(partitions_);

  CF_EXPECT(!composite_disk_path_.empty(), "No composite disk path");
  auto composite_mod_time =
      FileModificationTime(composite_disk_path_)
          .value_or(std::chrono::system_clock::time_point());

  if (composite_mod_time == decltype(composite_mod_time)()) {
    LOG(DEBUG) << "No prior composite disk";
    return true;
  } else if (last_component_mod_time > composite_mod_time) {
    LOG(DEBUG) << "Composite disk component file updated";
    return true;
  }

  return false;
}

Result<bool> DiskBuilder::BuildCompositeDiskIfNecessary() {
  if (!entire_disk_.empty()) {
    LOG(DEBUG) << "No composite disk to build";
    return false;
  }
  if (!CF_EXPECT(WillRebuildCompositeDisk())) {
    return false;
  }

  CF_EXPECT(vm_manager_ != VmmMode::kUnknown);
  // TODO: b/346855591 - run with QEMU when crosvm block device is integrated
  if (vm_manager_ == VmmMode::kCrosvm) {
    CF_EXPECT(!header_path_.empty(), "No header path");
    CF_EXPECT(!footer_path_.empty(), "No footer path");
    CF_EXPECT(CreateCompositeDisk(
        partitions_, AbsolutePath(header_path_), AbsolutePath(footer_path_),
        AbsolutePath(composite_disk_path_), read_only_));
  } else {
    // If this doesn't fit into the disk, it will fail while aggregating. The
    // aggregator doesn't maintain any sparse attributes.
    CF_EXPECT(AggregateImage(partitions_, AbsolutePath(composite_disk_path_)));
  }

  using android::base::WriteStringToFile;
  CF_EXPECT(WriteStringToFile(CF_EXPECT(TextConfig()), config_path_), true);

  return true;
}

Result<bool> DiskBuilder::BuildOverlayIfNecessary() {
#ifdef __APPLE__
  return false;
#else
  bool can_reuse_overlay = resume_if_possible_;

  CF_EXPECT(!overlay_path_.empty(), "Overlay path missing");
  auto overlay_mod_time =
      FileModificationTime(overlay_path_)
          .value_or(std::chrono::system_clock::time_point());

  CF_EXPECT(!composite_disk_path_.empty(), "Composite disk path missing");
  auto composite_disk_mod_time =
      FileModificationTime(composite_disk_path_)
          .value_or(std::chrono::system_clock::time_point());
  if (overlay_mod_time == decltype(overlay_mod_time)()) {
    LOG(DEBUG) << "No prior overlay";
    can_reuse_overlay = false;
  } else if (overlay_mod_time < composite_disk_mod_time) {
    LOG(DEBUG) << "Overlay is out of date";
    can_reuse_overlay = false;
  }

  if (can_reuse_overlay) {
    return false;
  }

  CF_EXPECT(!crosvm_path_.empty(), "crosvm binary missing");
  CF_EXPECT(
      Qcow2Image::Create(crosvm_path_, composite_disk_path_, overlay_path_));

  return true;
#endif
}

}  // namespace cuttlefish
