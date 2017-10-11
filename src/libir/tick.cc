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

#include "libir/tick.h"

namespace iracing {

IracingTick::IracingTick(char* data, int length) {
  data_.assign(data, data + length);
}

bool IracingTick::IsValid() {
  if (data_.size() == 0) {
    return false;
  }
  return true;
}


}  // namespace iracing
