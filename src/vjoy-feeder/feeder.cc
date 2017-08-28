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

#include "vjoy-feeder/feeder.h"

#include <fstream>
#include <ios>

#include "json.hpp"

using json = nlohmann::json;

namespace ecu {
namespace vjoy {

// Maximum number of input buttons on a vJoy device
static const uint8_t kMaxDeviceButtons = 128;

// Maximum number of available vJoy devices
static const uint8_t kMaxDevices = 16;

/**
   Acquires a vJoy device.
   If paramater device_number is specified, only that device will
   be acquired. Otherwise try acquiring a device from all 13 devices.

   @param[in] device_number Device to acquire. Optional
  */
bool Feeder::AcquireDevice(uint8_t device_number) {
  if (!vJoyEnabled()) {
    LOGF(g3::FATAL, "vJoy is not installed/enabled. Failed to create feeder");
  }

  unsigned short ver_dll, ver_drv = 0;
  if (!DriverMatch(&ver_dll, &ver_drv)) {
    LOGF(g3::FATAL, "vJoy Driver (version %04x) does not match "      \
         "vJoyInterface DLL (version %04x)\n", ver_drv, ver_dll);
  }

  if (device_number != 0) {
    VjdStat status = GetVJDStatus(device_number);
  
    if ((status == VJD_STAT_OWN) ||
        ((status == VJD_STAT_FREE) && (!AcquireVJD(device_number)))) {
      LOGF(g3::WARNING, "Failed to acquire vJoy device number %d.\n", device_number);
      return false;
    }
    else {
      LOGF(g3::DEBUG, "Acquired vJoy device number %d.\n", device_number);
      return true;
    }
  }

  // TODO:(garciaadrian): irFFB uses device 1 so don't try acquiring it
  for (auto i = 2; i < kMaxDevices; ++i) {
    VjdStat status = GetVJDStatus(i);
  
    if ((status == VJD_STAT_OWN) ||
        ((status == VJD_STAT_FREE) && (!AcquireVJD(i)))) {
      LOGF(g3::WARNING, "Failed to acquire vJoy device number %d.\n", i);
    }
    else {
      LOGF(g3::DEBUG, "Acquired vJoy device number %d.\n", i);
      device_number_ = i;
      return true;
    }
  }

  return false;
}

/**
   Attempts to deserialize a feeder json configuration 'filename'
   then acquire vjoy device N from configuration
   
   @param[in] filename feeder json configuration file
 */
Feeder::Feeder(const std::string& filename) {
  std::ifstream joy(filename, std::ios::binary | std::ios::in);

  if (!joy) {
    LOGF(g3::DEBUG, "vJoy configuration %s not found.\n", filename.c_str());
  }
  
  else {
    try {
      json j;
      joy >> j;

      for (json::iterator it = j["input_map_"].begin();
           it != j["input_map_"].end();
           ++it) {
      
        auto idx = it - j["input_map_"].begin();
        if (idx > static_cast<int>(input_map_.size() - 1)) {
          break;
        }
        input_map_.at(idx) = *it;
      }
    
      device_number_ = j["device_number_"];
      assert(device_number_ < kMaxDevices);
      
      if (!AcquireDevice(device_number_)) {
        LOGF(g3::FATAL, "vJoy device %d is disabled or owned by another program.\n",
             device_number_);
      }
      
    } catch (...) {
      LOGF(g3::WARNING, "Failed to load %s\n", filename.c_str());
    }
  }
  
  if (!Acquired()) {
    AcquireDevice();    
  }

  ResetVJD(device_number_);
  ResetButtons(device_number_);  
}

Feeder::~Feeder() {
  if (Acquired()) {
    RelinquishVJD(device_number_);
    Serialize("joy.json");
  }
  consumer_.Quit();
  consumer_.AwaitQuit();
};

/**
   Serializes device_number_ and input_map_ into a json
   string. Truncate file 'filename' then write string to file
 */
void Feeder::Serialize(const std::string& filename) {
  std::ofstream joy(filename, std::ios::binary | std::ios::out | std::ios_base::trunc);
  json j;
  j["device_number_"] = device_number_;
  j["input_map_"] = input_map_;
  auto string = j.dump();
  joy.write(string.c_str(), string.length());
  
}

/**
   If input is in the input_map_ then send a vJoy position
   report with corresponding button to consumer_
   No input axis/pedal support for now
 */
void Feeder::ExecInput(const iracing::iRacingInput input) {
  for (auto it = input_map_.begin(); it != input_map_.end(); ++it) {
    if (it->id_ == input.id_) {
      JOYSTICK_POSITION_V2 report = {0};
      report.bDevice = device_number_;

      if (input.id_ <= 32) {
        report.lButtons |= input.id_;
      }
      else if (input.id_ <= 64) {
        report.lButtonsEx1 |= input.id_;
      }
      else if (input.id_ <= 96) {
        report.lButtonsEx2 |= input.id_;
      }
      else if (input.id_ <= 128) {
        report.lButtonsEx3 |= input.id_;
      }
      consumer_.PostPosition(report);
            
      break;
    }
  }
}

/**
   If a vJoy device is acquired then reset all axis and button
   position data to 0
 */
void Feeder::Reset() {
  if (Acquired()) {
    ResetVJD(device_number_);
    ResetButtons(device_number_);
  }
}

/**
   Returns true if vJoy device device_number_ is acquired by us
 */
bool Feeder::Acquired() {
  return GetVJDStatus(device_number_) == VJD_STAT_OWN;
};

/**
   Map a vJoy device button to an iRacingInput
 */
void Feeder::SetButtonInput(const uint8_t button, const iracing::iRacingInput input) {
  if (button > kMaxDeviceButtons) {
    return;
  }

  input_map_[button] = input;
}


}  // namespace vjoy
}  // namespace ecu
