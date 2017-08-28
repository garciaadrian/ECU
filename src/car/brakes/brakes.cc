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

#include "car/brakes/brakes.h"
#include "car/brakes/console_commands.h"
#include "libir/iracing.h"

namespace ecu {
namespace car {

BrakeSystem::BrakeSystem() {};
BrakeSystem::~BrakeSystem() {};

float GetSetupBaseBias(const std::string& session_info) {
  auto base_bias =
      ::iracing::ParseSessionInfo(session_info, "CarSetup:BrakeDesignInCar:BaseBrakeBias:");
  
  if (base_bias.empty()) {
    return 0.0f;
  }
  
  base_bias = base_bias.substr(0, 4);
  
  return stof(base_bias);
}

void BrakeSystem::AddBrakingZone(BrakingZone zone) {
  zones_.push_back(zone);
}

void BrakeSystem::GetState(::iracing::IracingTick& tick,
                           vjoy::Feeder& device,
                           const std::string& session_info) {
  float lap_dist = tick.GetChannel<float>("LapDist");
  float bias = tick.GetChannel<float>("dcBrakeBias");

  float base_bias = GetSetupBaseBias(session_info);
  float current_bias = base_bias + bias;

  // TODO(garciaadrian): We might get stuck waiting for the consumer
  // to process our commands
  
  if (!unserviced_requests_.empty()) {
    const auto& req = unserviced_requests_.front();
    if (req == current_bias) {
      unserviced_requests_.pop();
    }
    else {
      return;
    }
  }
  
  for (auto& zone : zones_) {
    if (lap_dist > zone.zone_start &&
        lap_dist < zone.zone_end) {
      if (current_bias > zone.bias) {
        device.ExecInput(iracing::BRAKE_BIAS_DEC);
        unserviced_requests_.push(current_bias - 0.25f);
      }

      else if (current_bias < zone.bias) {
        device.ExecInput(iracing::BRAKE_BIAS_INC);
        unserviced_requests_.push(current_bias + 0.25f);
      }
    }
  }
}

std::vector<std::unique_ptr<ConCommand>> BrakeSystem::CreateConsoleCommands() {
  std::vector<std::unique_ptr<ConCommand>> commands;
  commands.push_back(std::make_unique<ChangeBiasCommand>(this));
  
  return commands;
}

}  // namespace car
}  // namespace ecu
