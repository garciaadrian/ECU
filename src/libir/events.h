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

#include <stdint.h>

#include "libir/tick.h"

#ifndef LIBIR_EVENTS_H_
#define LIBIR_EVENTS_H_

namespace iracing {

class IracingEvent {
 public:
  IracingEvent() {};
  virtual ~IracingEvent() {};
};

class OnDataEvent : public IracingEvent {
 public:
  explicit OnDataEvent(IracingTick tick);
  ~OnDataEvent() {};

  IracingTick tick() { return tick_; }
 private:
  IracingTick tick_;
};

}  // namespace iracing

#endif //  LIBIR_EVENTS_H_

