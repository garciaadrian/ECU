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

#include "ffb/di_manager.h"

#include <iostream>
#include <QDebug>
#include <system_error>

namespace ecu {
namespace joy {

inline BOOL CALLBACK DeviceManager::StaticEnumFFDevicesCallback(
    LPCDIDEVICEINSTANCE diDevInst, VOID* pvRef) {
  DeviceManager* p_this = reinterpret_cast<DeviceManager*>(pvRef);
  return p_this->EnumFFDevicesCallback(diDevInst, nullptr);
}

BOOL DeviceManager::EnumFFDevicesCallback(LPCDIDEVICEINSTANCE diDevInst,
                                          VOID* pvRef) {
  UNREFERENCED_PARAMETER(pvRef);

  if (wcscmp(diDevInst->tszProductName, L"vJoy Device") == 0) {
    return DIENUM_CONTINUE;
  }

  try {
    devices_.push_back(new Device(direct_input_, diDevInst, window_, this));
  } catch (const std::system_error& error) {
    qDebug() << "Failed to create device:"
             << QString::fromWCharArray(diDevInst->tszProductName);
    qDebug() << "Error:" << error.what()
             << QString::number(error.code().value(), 16);
    return DIENUM_CONTINUE;
  } catch (const std::error_code& error) {
    qDebug() << "Error:" << QString::fromStdString(error.message());
    return DIENUM_CONTINUE;
  }

  emit DeviceConnected(QString::fromWCharArray(diDevInst->tszProductName));
  connect(devices_.back(), &Device::DeviceUnresponsive, this,
          &DeviceManager::DeviceUnresponsive);
  devices_.back()->start(QThread::HighPriority);

  return DIENUM_CONTINUE;
}

void DeviceManager::Enumerate() {
  direct_input_->EnumDevices(DI8DEVCLASS_GAMECTRL,
                             DeviceManager::StaticEnumFFDevicesCallback,
                             reinterpret_cast<DeviceManager*>(this),
                             DIEDFL_ATTACHEDONLY | DIEDFL_FORCEFEEDBACK);
}

void DeviceManager::DeviceUnresponsive(const QString& device_name) {
  for (const auto& device : devices_) {
    if (device->DeviceName() == device_name) {
      device->StopFFB();
      emit DeviceDisconnected(device_name);
    }
  }
}

void DeviceManager::SetCurrentDevice(const QString& device_name) {
  for (const auto& device : devices_) {
    if (device->DeviceName() != device_name) {
      device->StopFFB();
    } else {
      device->StartFFB();
      break;
    }
  }
}

void DeviceManager::SetMaxForce(const int& force) {
  // TODO(garciaadrian):
  // maybe unnecessary but check if force is within bounds (1-50) Nm ?
  // also notify iracing of new max force through broadcastMsg

  qDebug() << "Setting max force to:" << force << "Nm";
}

DeviceManager::DeviceManager(HWND window) : QObject(nullptr), window_(window) {
  if (FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION,
                                IID_IDirectInput8, (void**)&direct_input_,
                                nullptr))) {
    throw std::runtime_error("Failed to create a direct input device");
  }
}

DeviceManager::~DeviceManager() {
  for (const auto& device : devices_) {
    device->StopFFB();
  }

  if (direct_input_) {
    direct_input_->Release();
  }
}

}  // namespace joy
}  // namespace ecu
