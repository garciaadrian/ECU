/**
*******************************************************************************
*                                                                             *
* ECU: iRacing Extensions Collection Project                                  *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#include <memory>
#include <queue>

#include "libir/tick.h"
#include "base/console_command.h"
#include "hid/input_command.h"
#include "vjoy-feeder/feeder.h"

namespace ecu {
namespace car {

struct BrakingZone {
  uint16_t zone_start;
  uint16_t zone_end;
  float bias;
};

class BrakeSystem {
 public:
  BrakeSystem();
  ~BrakeSystem();
  
  void GetState(::iracing::IracingTick& tick, vjoy::Feeder& device,
                const std::string& session_info);
  void AddBrakingZone(BrakingZone zone);

  std::vector<std::unique_ptr<ConCommand>> CreateConsoleCommands();
  std::vector<std::unique_ptr<hid::InputCommand>> CreateInputCommands();
  
 private:
  std::queue<float> unserviced_requests_;
  std::vector<BrakingZone> zones_;
};

}  // namespace car
}  // namespace ecu
