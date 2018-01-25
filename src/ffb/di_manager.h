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

#ifndef FFB_DI_DEVICE_MANAGER_H_
#define FFB_DI_DEVICE_MANAGER_H_

#include <vector>
#include <QObject>

#ifdef DIRECTINPUT_VERSION
#undef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <dinput.h>

#include "ffb/di_device.h"

namespace ecu {
namespace joy {

class DeviceManager : public QObject {
  Q_OBJECT
 public:
  DeviceManager(HWND window);
  ~DeviceManager();
  static BOOL CALLBACK
  StaticEnumFFDevicesCallback(LPCDIDEVICEINSTANCE diDevInst, VOID* pvRef);
  BOOL EnumFFDevicesCallback(LPCDIDEVICEINSTANCE diDevInst, VOID* pvRef);

  void Enumerate();
  void SetCurrentDevice(const QString& device_name);
  void SetMaxForce(const int& force);
 signals:
  void DeviceConnected(const QString& device);
  void DeviceDisconnected(const QString& device);
 public slots:
  void DeviceUnresponsive(const QString& device_name);

 private:
  LPDIRECTINPUT8 direct_input_ = nullptr;
  LPDIRECTINPUTDEVICE8 ff_device_ = nullptr;
  std::vector<Device*> devices_;
  HWND window_ = nullptr;
};

}  // namespace joy
}  // namespace ecu

#endif  // FFB_DI_DEVICE_H_
