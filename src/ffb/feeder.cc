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

#include "ffb/feeder.h"

#include <iostream>

#include <Windows.h>

#include "vJoydriver/inc/public.h"
#include "vJoydriver/inc/vjoyinterface.h"

namespace {
using namespace ecu::joy;

struct FeederErrCategory : std::error_category {
  const char* name() const noexcept override;
  std::string message(int _Errval) const override;
};

const char* FeederErrCategory::name() const noexcept { return "Feeder"; }

std::string FeederErrCategory::message(int _Errval) const {
  switch (static_cast<FeederErrc>(_Errval)) {
    case FeederErrc::VJoyNotEnabled: {
      return "vJoy is not installed or enabled.";
    }
    case FeederErrc::VJoyDriverMismatch: {
      return "vJoyinterface dll version does not match the version packaged "
             "with ECU.";
    }
    case FeederErrc::VJoyAcquireFail: {
      return "Failed to acquire a vJoy device.";
    }
    case FeederErrc::VJoyGetDeviceFail: {
      return "Failed to query max number of vJoy devices.";
    }
    default: { return "(unrecognized error)"; }
  }
}

const FeederErrCategory theFeederErrCategory;

}  // namespace

namespace ecu {
namespace joy {

std::error_code make_error_code(FeederErrc e) {
  return {static_cast<int>(e), theFeederErrCategory};
}

Feeder::Feeder() {
  if (!vJoyEnabled()) {
    throw std::error_code(FeederErrc::VJoyNotEnabled);
  }

  unsigned short ver_dll, ver_drv = 0;
  if (!DriverMatch(&ver_dll, &ver_drv)) {
    throw std::error_code(FeederErrc::VJoyDriverMismatch);
  }

  int max_devices = 0;
  if (!GetvJoyMaxDevices(&max_devices)) {
    throw std::error_code(FeederErrc::VJoyGetDeviceFail);
  }

  for (int i = 1; i <= max_devices; i++) {
    if (AcquireDevice(i)) {
      break;
    }
  }

  if (device_ == 0) {
    throw std::error_code(FeederErrc::VJoyAcquireFail);
  }

  ResetVJD(device_);
}

Feeder::~Feeder() {
  if (Acquired(device_)) {
    ResetVJD(device_);
    RelinquishVJD(device_);
  }
}

bool Feeder::AcquireDevice(const int device) {
  VjdStat status = GetVJDStatus(device);

  switch (status) {
    case VJD_STAT_OWN: {
      return true;
      break;
    }
    case VJD_STAT_FREE: {
      if (!IsDeviceFfbEffect(device, HID_USAGE_CONST) ||
          !IsDeviceFfbEffect(device, HID_USAGE_SINE) ||
          !IsDeviceFfbEffect(device, HID_USAGE_DMPR) ||
          !IsDeviceFfbEffect(device, HID_USAGE_FRIC) ||
          !IsDeviceFfbEffect(device, HID_USAGE_SPRNG) ||
          !GetVJDAxisExist(device, HID_USAGE_X)) {
        return false;
      }
      int ret = AcquireVJD(device);
      device_ = device;
      return ret;
      break;
    }
    default: {
      return false;
      break;
    }
  }
}

bool Acquired(const int& device) {
  return GetVJDStatus(device) == VJD_STAT_OWN;
}

bool Reset(const int& device) {
  if (Acquired(device)) {
    int ret = ResetVJD(device);
    ResetButtons(device);
    return ret;
  }
  return false;
}

}  // namespace joy
}  // namespace ecu
