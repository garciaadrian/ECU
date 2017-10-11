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

#ifndef LIBIR_TICK_H_
#define LIBIR_TICK_H_

#include <vector>

#include "irsdk/irsdk_defines.h"

namespace iracing {

class IracingTick {
 public:
  IracingTick(char* data, int length);

  std::vector<char> GetData() { return data_; }

  bool IsValid();
  
  template<typename T>
  T GetChannel(std::string name) {
    
    if (data_.size() == 0) {
      return (T)0;
    }
    
    int offset = irsdk_varNameToOffset(name.c_str());
    return *(T*)(data_.data() + offset);
  }
  
 private:
  std::vector<char> data_;
};

}  // namespace iracing

#endif // LIBIR_TICK_H_
