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

#ifndef FFB_FEEDER_H_
#define FFB_FEEDER_H_

#include <system_error>

namespace ecu {
namespace joy {

enum class FeederErrc {
  VJoyNotEnabled = 10,
  VJoyDriverMismatch,
  VJoyAcquireFail,
  VJoyGetDeviceFail
};

std::error_code make_error_code(FeederErrc);

class Feeder {
 public:
  Feeder();
  ~Feeder();

  const int DeviceNumber() const { return device_; }

 private:
  int device_ = 0;

  bool AcquireDevice(const int device);
};

bool Acquired(const int& device);
bool Reset(const int& device);

}  // namespace joy
}  // namespace ecu

namespace std {
template <>
struct is_error_code_enum<ecu::joy::FeederErrc> : true_type {};
}  // namespace std

#endif  // FFB_FEEDER_H_
