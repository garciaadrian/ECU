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

#ifndef VJOY_FEEDER_FEEDER_H_
#define VJOY_FEEDER_FEEDER_H_

#include <stdint.h>
#include <Windows.h> // for public.h
#include <unordered_map>
#include <queue>
#include <array>
#include "stdint.h"

#include "g3log/g3log.hpp"
#include "vJoydriver/inc/public.h"
#include "vJoydriver/inc/vjoyinterface.h"

#include "vjoy-feeder/joy_consumer.h"
#include "libir/iracing_input.h"


namespace ecu {
namespace vjoy {

class Feeder {
 public:
  Feeder();
  Feeder(const std::string& filename);
  ~Feeder();

  bool Acquired();
  bool AcquireDevice(uint8_t device_number);
  void SetButtonInput(const uint8_t button, const iracing::iRacingInput input);
  void Reset();
  void ExecInput(const iracing::iRacingInput input);
  void LoadConfiguration(const std::string& filename);
  void Serialize(const std::string& filename);

 private:
  uint8_t device_number_;
  std::array<iracing::iRacingInput, 128> input_map_ = { {0, ""} };
  JoyConsumer consumer_;

};

void SetDefaultDeviceButtons(Feeder& device);

}  // namespace vjoy
}  // namespace ecu

#endif // VJOY_FEEDER_FEEDER_H_

