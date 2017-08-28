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

#include "base/console_command.h"

namespace ecu {
namespace car {

class ChangeBiasCommand : public ConCommand {
 public:
  ChangeBiasCommand(BrakeSystem* brake_system) : brake_system_(brake_system) {
    name_ = "changebias";
    description_ = "Change brake bias";
  }
  ~ChangeBiasCommand() {};

  void execute(const std::vector<std::string>& params) final override {
    if (params.size() != 3) {
      return;
    }
    if (!bias_valid(params[0])) {
      return;
    }
    
    const float bias = std::stof(params[0]);
    
    BrakingZone zone;

    zone.bias = bias;
    zone.zone_start = static_cast<uint16_t>(std::stoi(params[1]));
    zone.zone_end = static_cast<uint16_t>(std::stoi(params[2]));
    
    brake_system_->AddBrakingZone(zone);
  }

 private:
  BrakeSystem* brake_system_;
  
  bool bias_valid(const std::string& bias) {
    
    float biasf = 0.0f;
    
    try {
      float biasf = std::stof(bias);
    }

    catch (std::invalid_argument e) {
      LOGF(g3::DEBUG, "Invalid bias: %s", bias);
      return false;
    }

    catch (std::out_of_range e) {
      LOGF(g3::DEBUG, "bias out of range: %s", bias);
      return false;
    }
    // bias range on the mp4-30
    if (biasf > 59.5 ||
        biasf < 53.5) {
      return false;
    }
    
    return true;
  }
};

}  // namespace car
}  // namespace ecu
