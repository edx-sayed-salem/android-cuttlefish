/*
 * Copyright 2023 The Android Open Source Project
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

#include <vector>

#include "cuttlefish/common/libs/fs/shared_buf.h"
#include "cuttlefish/common/libs/fs/shared_fd.h"
#include "cuttlefish/common/libs/utils/result.h"

#include "cuttlefish/host/commands/casimir_control_server/rf_packets.h"

namespace cuttlefish {

using namespace casimir::rf;

class CasimirController {
 public:
  static Result<CasimirController> ConnectToTcpPort(int rf_port);
  static Result<CasimirController> ConnectToUnixSocket(const std::string& rf);

  Result<void> Mute();
  Result<void> Unmute();

  Result<void> SetPowerLevel(uint32_t power_level);

  Result<std::tuple<std::vector<uint8_t>, std::string, bool, uint32_t, uint32_t,
                    uint32_t, double>>
  SendBroadcast(std::vector<uint8_t> data, std::string type, bool crc,
                uint8_t bits, uint32_t bitrate, uint32_t timeout, double power);

  /*
   * Poll for NFC-A + ISO-DEP
   */
  Result<uint16_t> Poll();

  Result<std::vector<uint8_t>> SendApdu(uint16_t receiver_id,
                                        std::vector<uint8_t> apdu);

 private:
  CasimirController(SharedFD sock);
  /*
   * Select NFC-A, and returns sender id.
   */
  Result<uint16_t> SelectNfcA();

  /*
   * Select T4AT by choosing NFC-A listener using ISO-DEP protocol (Type-4A Tag
   * platform).
   */
  Result<void> SelectT4AT(uint16_t sender_id);

  Result<void> Write(const RfPacketBuilder& rf_packet);
  Result<std::shared_ptr<std::vector<uint8_t>>> ReadExact(
      size_t size, std::chrono::microseconds timeout);
  Result<std::shared_ptr<std::vector<uint8_t>>> ReadRfPacket(
      std::chrono::microseconds timeout);

  SharedFD sock_;
  uint8_t power_level;
};

}  // namespace cuttlefish
