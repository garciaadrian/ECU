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
#include "ffb/di_device.h"

#include "timeapi.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <QtDebug>
#include <system_error>

#include "ffb/feeder.h"

#define RESOLUTION 5

namespace ecu {
namespace joy {

// Throw a std::system_error if the HRESULT indicates failure.
template <typename T>
void ThrowIfFailed(HRESULT hr, T&& msg) {
  if (FAILED(hr))
    throw std::system_error{hr, std::system_category(), std::forward<T>(msg)};
}

inline BOOL CALLBACK Device::StaticEnumObjectCallback(
    LPCDIDEVICEOBJECTINSTANCE diDevInst, VOID* pvRef) {
  UNREFERENCED_PARAMETER(diDevInst);

  auto ref = reinterpret_cast<axis*>(pvRef);
  (*ref)++;
  return DIENUM_CONTINUE;
}

inline void CALLBACK Device::StaticFfbThread(PVOID FfbPacket, PVOID user_data) {
  Device* p_this = reinterpret_cast<Device*>(user_data);
  p_this->AddFfbPacket(static_cast<PFFB_DATA>(FfbPacket));
}

const QString Device::DeviceName() {
  return QString::fromWCharArray(device_inst_->tszProductName);
}

void Device::StopFFB() { exit_ = true; }
void Device::StartFFB() {}

void Device::AddFfbPacket(PFFB_DATA FfbPacket) {
  packets_.enqueue(static_cast<FFB_DATA>(*FfbPacket));
}

void Device::run() {
  MMRESULT result = timeBeginPeriod(RESOLUTION);
  while (!exit_) {
    // Write button, axis, and pov data to vjoy
    // Read ffb data from vjoy, Modify then write ffb data to wheel

    JOYSTICK_POSITION data = {0};
    ResetVJD(feeder_.DeviceNumber());

    DIJOYSTATE joy_state = {0};
    HRESULT hr = device_->GetDeviceState(sizeof(joy_state), &joy_state);
    if (hr != DI_OK) {
      // emit device failed? or try to reacquire?
      emit DeviceUnresponsive(
          QString::fromWCharArray(device_inst_->tszProductName));
      break;
    }

    data.wAxisX = joy_state.lX;
    data.wAxisY = joy_state.lY;
    data.wAxisZ = joy_state.lZ;

    for (int i = 0; i < num_buttons_; i++) {
      if (joy_state.rgbButtons[i])
        data.lButtons |= 1 << i;
      else
        data.lButtons &= ~(1 << i);
    }

    for (int i = 0; i < pov_buttons_; i++) {
      switch (i) {
        case 0: {
          data.bHats = joy_state.rgdwPOV[i];
          break;
        }
        case 1: {
          data.bHatsEx1 = joy_state.rgdwPOV[i];
          break;
        }
        case 2: {
          data.bHatsEx2 = joy_state.rgdwPOV[i];
          break;
        }
        case 3: {
          data.bHatsEx3 = joy_state.rgdwPOV[i];
          break;
        }
        default: { break; }
      }
    }

    UpdateVJD(feeder_.DeviceNumber(), (PVOID)&data);

    FFB_DATA packet{};

    int ret = packets_.try_dequeue(packet);

    if (ret) {
      int mag = (packet.data[3] << 8) + packet.data[2];
      mag /= 4;
      force_.lOffset = mag;
      hr = effect_->SetParameters(&dieff_,
                                  DIEP_TYPESPECIFICPARAMS | DIEP_NORESTART);
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  result = timeEndPeriod(RESOLUTION);
}

Device::Device(LPDIRECTINPUT8 direct_input, LPCDIDEVICEINSTANCE diDevInst,
               HWND window, QObject* parent)
    : QThread(parent), device_inst_(diDevInst) {
  HRESULT hr;

  hr = direct_input->CreateDevice(diDevInst->guidInstance, &device_, nullptr);

  ThrowIfFailed(hr, "Failed to get interface to device");

  hr = device_->SetDataFormat(&c_dfDIJoystick);

  ThrowIfFailed(hr, "Failed to set data format on device");

  hr = device_->SetCooperativeLevel(window, DISCL_EXCLUSIVE | DISCL_BACKGROUND);

  ThrowIfFailed(hr, "Failed to set device cooperative level.");

  hr = device_->Acquire();

  ThrowIfFailed(hr, "Failed to acquire device");

  hr = device_->EnumObjects(Device::StaticEnumObjectCallback,
                            (void*)&num_buttons_, DIDFT_BUTTON);

  ThrowIfFailed(hr, "Failed to get device buttons");

  hr = device_->EnumObjects(Device::StaticEnumObjectCallback,
                            (void*)&pov_buttons_, DIDFT_POV);

  ThrowIfFailed(hr, "Failed to get device POV buttons");

  force_.dwPeriod = INFINITE;

  dieff_.dwSize = sizeof(DIEFFECT);
  dieff_.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
  dieff_.dwDuration = INFINITE;
  dieff_.dwSamplePeriod = 0;
  dieff_.dwGain = DI_FFNOMINALMAX;
  dieff_.dwTriggerButton = DIEB_NOTRIGGER;
  dieff_.dwTriggerRepeatInterval = 0;
  dieff_.cAxes = 1;
  dieff_.rgdwAxes = axis_;
  dieff_.rglDirection = direction_;
  dieff_.lpEnvelope = NULL;
  dieff_.cbTypeSpecificParams = sizeof(DIPERIODIC);
  dieff_.lpvTypeSpecificParams = &force_;
  dieff_.dwStartDelay = 0;

  hr = device_->CreateEffect(GUID_Sine, &dieff_, &effect_, nullptr);

  ThrowIfFailed(hr, "Failed to create FFB effect");

  hr = effect_->SetParameters(&dieff_, DIEP_TYPESPECIFICPARAMS | DIEP_START);

  if (hr == DIERR_NOTINITIALIZED || hr == DIERR_INPUTLOST ||
      hr == DIERR_INCOMPLETEEFFECT || hr == DIERR_INVALIDPARAM) {
    ThrowIfFailed(hr, "Error setting parameters of FFB effect");
  }

  // 360 packets = 6 seconds of ffb data
  packets_ = moodycamel::ReaderWriterQueue<FFB_DATA>(360);

  FfbRegisterGenCB(Device::StaticFfbThread, this);
}

Device::~Device() {
  if (device_) {
    device_->Unacquire();
  }

  if (effect_) {
    effect_->Stop();
    effect_->Release();
  }

  QThread::wait();
}

}  // namespace joy
}  // namespace ecu
