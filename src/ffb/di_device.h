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

#ifndef FFB_DI_DEVICE_H_
#define FFB_DI_DEVICE_H_

#include "dinput.h"
#include "stdint.h"
#include <atomic>
#include <QWaitCondition>
#include <QThread>

#include "vJoydriver/inc/public.h"
#include "vJoydriver/inc/vjoyinterface.h"

#include "ffb/feeder.h"
#include "ffb/readerwriterqueue.h"

namespace ecu {
namespace joy {

using axis = uint_least64_t;

class Device : public QThread {
  Q_OBJECT
 public:
  Device(LPDIRECTINPUT8 direct_input, LPCDIDEVICEINSTANCE diDevInst,
         HWND window, QObject* parent = nullptr);
  ~Device();

  static BOOL CALLBACK
  StaticEnumObjectCallback(LPCDIDEVICEOBJECTINSTANCE diDevInst, VOID* pvRef);

  static void CALLBACK StaticFfbThread(PVOID FfbPacket, PVOID user_data);
  void AddFfbPacket(PFFB_DATA FfbPacket);

  LPDIRECTINPUTDEVICE8 DeviceHandle() const { return device_; }

  void StopFFB();
  void StartFFB();

  const QString DeviceName();

 protected:
  void run() override;

 signals:
  void DeviceUnresponsive(const QString& device);

 private:
  LPDIRECTINPUTDEVICE8 device_ = nullptr;
  LPCDIDEVICEINSTANCE device_inst_ = nullptr;

  axis ff_axis_ = 0;
  axis num_buttons_ = 0;
  axis pov_buttons_ = 0;

  DWORD axis_[1] = {DIJOFS_X};
  LONG direction_[1] = {0};
  DIPERIODIC force_ = {0};
  DIEFFECT dieff_ = {0};

  LPDIRECTINPUTEFFECT effect_ = nullptr;

  ecu::joy::Feeder feeder_;

  std::atomic<bool> exit_{0};
  QWaitCondition wait_;

  moodycamel::ReaderWriterQueue<FFB_DATA> packets_;
};

}  // namespace joy
}  // namespace ecu

#endif  // FFB_DI_DEVICE_H_
