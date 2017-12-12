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

#ifndef LIBIR_IRACING_H_
#define LIBIR_IRACING_H_

#include <memory>
#include <functional>
#include <vector>

#include "libir/events.h"
#include "libir/tick.h"

namespace iracing {

enum class Event {
  ON_DATA, // Called on each game tick, aprox. 17ms
};

template<class T>
using Pimpl =  std::unique_ptr<T>;

std::string ParseSessionInfo(const std::string& session_info,
                             const std::string& path);

class Iracing {
 public:
  Iracing();
  ~Iracing();
  
  Iracing(Iracing && op) noexcept;
  Iracing& operator=(Iracing && op) noexcept;
  
  bool connect();
  void loop();
  bool IsReplay();
  IracingTick GetTick();
  std::string GetSessionInfo();

  void AddListener(std::function<void(IracingEvent event)> callback, Event event);

 private:
  
  class IracingImpl;
  Pimpl<IracingImpl> impl_;
};



}  // namespace iracing

#endif // LIBIR_IRACING_H_
