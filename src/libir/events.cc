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

#include "libir/events.h"

namespace iracing {

OnDataEvent::OnDataEvent(IracingTick tick)
    : tick_(tick) {
  
}

}  // namespace iracing
