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

#include "cuttlefish/host/commands/cvd/unittests/selector/creation_analyzer_helper.h"

#include <android-base/strings.h>

#include "cuttlefish/host/commands/cvd/cli/types.h"

namespace cuttlefish {
namespace selector {
namespace {

// copied from server.h
struct CommandInvocation {
  std::string command;
  std::vector<std::string> arguments;
};

CommandInvocation MockParseInvocation(const std::vector<std::string>& args) {
  if (args.empty()) {
    return CommandInvocation{};
  }
  if (args[0] != "cvd") {
    return CommandInvocation{.command = args[0],
                             .arguments = cvd_common::Args{}};
  }
  if (args.size() == 1) {
    return CommandInvocation{.command = "help",
                             .arguments = cvd_common::Args{}};
  }
  cvd_common::Args program_args{args.begin() + 2, args.end()};
  return CommandInvocation{.command = args[1], .arguments = program_args};
}

}  // namespace

CreationInfoGenTest::CreationInfoGenTest() { Init(); }
void CreationInfoGenTest::Init() {
  const auto& input_param = GetParam();
  selector_args_ = android::base::Tokenize(input_param.selector_args, " ");
  auto cmd_invocation =
      MockParseInvocation(android::base::Tokenize(input_param.cmd_args, " "));
  sub_cmd_ = cmd_invocation.command;
  cmd_args_ = std::move(cmd_invocation.arguments);
  if (!input_param.home.empty()) {
    envs_["HOME"] = input_param.home;
  }
  if (!input_param.android_host_out.empty()) {
    envs_["ANDROID_HOST_OUT"] = input_param.android_host_out;
  }
  expected_output_ = input_param.expected_output.output;
  expected_success_ = input_param.expected_output.is_success;
}

}  // namespace selector
}  // namespace cuttlefish
