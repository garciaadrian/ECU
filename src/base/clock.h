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

#ifndef BASE_CLOCK_H_
#define BASE_CLOCK_H_

#include <chrono>

namespace ecu {

using frames = std::chrono::duration<double, std::milli>;

}  // namespace ecu

#endif // BASE_CLOCK_H_

