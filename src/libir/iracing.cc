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


#include "libir/iracing.h"
#include "libir/events.h"
#include "libir/tick.h"
#include "base/delegate.h"
#include "base/clock.h"

#include "irsdk/irsdk_defines.h"
#include "irsdk/irsdk_client.h"
#include "irsdk/yaml_parser.h"
#include "g3log/g3log.hpp"

#include <stdint.h>
#include <climits>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <string>

namespace iracing {

struct Listener {
  std::function<void(IracingEvent event)> callback;
  Event event;
};

class Iracing::IracingImpl {
  const irsdk_header* header_ = nullptr;
  char* live_data_ = nullptr;
  uint16_t live_data_size_ = 0;
  uint16_t timeout_ = 18;
  
  std::vector<IracingTick> ticks_;
  std::vector<Listener> listeners_;

  std::mutex ticks_lock_;
  std::mutex listener_lock_;
    
 public:
  Iracing::IracingImpl() {}
  ~IracingImpl() {
    if (live_data_ != nullptr) {
      free(live_data_);
    }
  }

  void loop();

  bool connect(Iracing* instance) {
    irsdkClient::instance().waitForData(1);
    return irsdk_isConnected();
  }

  void AddListener(std::function<void(IracingEvent event)> callback, Event event) {
    Listener listener;
    listener.callback = callback;
    listener.event = event;
    
    std::lock_guard<std::mutex> guard(listener_lock_);
    listeners_.push_back(listener);
  }

  void RemoveAllListeners() {
    std::lock_guard<std::mutex> guard(listener_lock_);
    listeners_.clear();
  }

  void SignalCallbacks(Event event) {
    std::lock_guard<std::mutex> listener_guard(listener_lock_);
    for (auto& listener : listeners_) {
      if (listener.event == event) {
        std::lock_guard<std::mutex> ticks_guard(ticks_lock_);
        listener.callback(OnDataEvent(ticks_.back()));
      }
    }
  }

  std::string GetSessionInfo() {
    if (header_) {
      return std::string(irsdk_getSessionInfoStr());
    }
    return std::string();
  }

  std::string GetSessionInfoParse(const std::string& path) {
    const int len = 4096; // Should be big enough for most session infos
    char result[len] = {0};
    
    int ret = irsdkClient::instance().getSessionStrVal(path.c_str(), result, len);

    return std::string(result);
  }
  
  IracingTick GetTick() {
    if (!header_) {
      bool is_initialized = irsdk_startup();
      if (!is_initialized) {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
      }
      header_ = irsdk_getHeader();
    }

    if (header_) {
      if (!live_data_ || live_data_size_ != header_->bufLen) {
        if (live_data_) {
          free(live_data_);
        }

        live_data_size_ = header_->bufLen;
        live_data_ = (char*)malloc(live_data_size_);
        irsdk_waitForDataReady(timeout_, live_data_);
      }

      else {
        if (!irsdk_isConnected()) {
          // iRacing has shutdown
          return IracingTick(nullptr, 0);
        }
      }
      
      if (irsdk_waitForDataReady(timeout_, live_data_)) {
        return IracingTick(live_data_, header_->bufLen);
      }
    }
    
    return IracingTick(nullptr, 0);
  }
};

void Iracing::IracingImpl::loop() {
  char* live_data = nullptr;
  header_ = irsdk_getHeader();
  
  uint16_t live_data_size = 0;
  uint32_t timeouts = 0;
  std::vector<Event> events;

  while (true) {
    auto start = std::chrono::steady_clock::now();
    
    if (header_) {
      if (!live_data || live_data_size != header_->bufLen) {
        
        if (live_data)
          free(live_data);
        
        live_data_size = header_->bufLen;
        live_data = (char*)malloc(live_data_size);
        irsdk_waitForDataReady(timeout_, live_data);
      }

      else {
        if (!irsdk_isConnected()) {
          // iRacing went from online to offline.
        }
        if (irsdk_waitForDataReady(timeout_, live_data)) {
          events.push_back(Event::ON_DATA);
          IracingTick tick(live_data, header_->bufLen);
                    
          ticks_lock_.lock();
          
          ticks_[0] = tick;
          
          ticks_lock_.unlock();
        }
      }
      
      for (auto& event : events) {
        SignalCallbacks(event);
      }
      

      events.clear();
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    LOGF(g3::DEBUG, "frametime:%f ms\n", ecu::frames(diff).count());
  }
}


std::string ParseSessionInfo(const std::string& session_info,
                             const std::string& path) {
  const char* tval = nullptr;
  int tval_len = 0;

  if (parseYaml(session_info.c_str(), path.c_str(), &tval, &tval_len)) {
    return std::string(tval);
  }
  return std::string();
}


Iracing::Iracing()
    :impl_(new IracingImpl()) {}

Iracing::~Iracing() = default;
Iracing::Iracing(Iracing && op) noexcept = default;
Iracing& Iracing::operator=(Iracing && op) noexcept = default;

bool Iracing::connect() {
  return impl_->connect(this);
}

void Iracing::loop() {
  impl_->loop();
}

bool Iracing::IsReplay() {
  return true;
}

IracingTick Iracing::GetTick() {
  return impl_->GetTick();
}

std::string Iracing::GetSessionInfo() {
  return impl_->GetSessionInfo();
}

void Iracing::AddListener(std::function<void(IracingEvent event)> callback, Event event) {
  impl_->AddListener(callback, event);
}

}  // namespace iracing
