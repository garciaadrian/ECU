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

#include "hid/g27/g27_input_driver.h"

#include <Windows.h>
#include <Hidsdi.h>
#include <SetupAPI.h>
#include <usbiodef.h>
#include <Winusb.h>
#include <fstream>
#include <stdlib.h>

namespace ecu {
namespace hid {
namespace g27 {

static constexpr GUID GUID_DEVINTERFACE_USB_DEVICE = {
    0xA5DCBF10L,
    0x6530,
    0x11D2,
    {0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED}};

G27InputDriver::G27InputDriver(ecu::ui::Window* window)
    : InputDriver(window) {
  bool device_registered = Register(window->hwnd());
  
  if (!device_registered) {
    return;
  }
}

G27InputDriver::~G27InputDriver() {}

bool G27InputDriver::Register(HWND window) {
  RAWINPUTDEVICE wheel_input = {0};
  wheel_input.usUsagePage = 1;
  wheel_input.usUsage = 4;
  wheel_input.dwFlags = RIDEV_INPUTSINK;
  wheel_input.hwndTarget = window;

  if (!RegisterRawInputDevices(&wheel_input, 1, sizeof(RAWINPUTDEVICE))) {
    return false;
  }

  return true;
}

std::pair<int, int> G27InputDriver::GetDeviceId() {
  return std::make_pair(this->kVendor_id_, this->kProduct_id_);
}

int G27InputDriver::GetState(ecu::ui::RawInputEvent* e) {
  // Get required size for RAWINPUT structure
  unsigned int buffer_size = 0;
  GetRawInputData((HRAWINPUT)e->lparam(), RID_INPUT, nullptr, &buffer_size,
                  sizeof(RAWINPUTHEADER));
  RAWINPUT* buffer = new RAWINPUT[buffer_size];

  GetRawInputData((HRAWINPUT)e->lparam(), RID_INPUT, buffer, &buffer_size,
                  sizeof(RAWINPUTHEADER));

  // Get required size for PHIDP_PREPARSED_DATA structure
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_PREPARSEDDATA, nullptr,
                        &buffer_size);
  PHIDP_PREPARSED_DATA preparsed_data =
      static_cast<PHIDP_PREPARSED_DATA>(std::malloc(buffer_size));
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_PREPARSEDDATA,
                        preparsed_data, &buffer_size);

  PHIDP_CAPS capabilities = new HIDP_CAPS[sizeof(HIDP_CAPS)];
  HidP_GetCaps(preparsed_data, capabilities);

  GUID device_guid;
  HidD_GetHidGuid(&device_guid);

  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_DEVICENAME, NULL,
                        &buffer_size);

  wchar_t* name = new wchar_t[buffer_size + 1];
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_DEVICENAME, name,
                        &buffer_size);
  HANDLE hid_handle =
      CreateFile(name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                 nullptr, OPEN_ALWAYS, NULL, nullptr);
  unsigned short capabilities_length = capabilities->NumberInputButtonCaps;

  PHIDP_BUTTON_CAPS button_capabilities =
      new HIDP_BUTTON_CAPS[capabilities_length];
  HidP_GetButtonCaps(HidP_Input, button_capabilities, &capabilities_length,
                     preparsed_data);

  uint8_t num_buttons = button_capabilities->Range.UsageMax -
                        button_capabilities->Range.UsageMin + 1;

  PHIDP_VALUE_CAPS value_capabilities =
      new HIDP_VALUE_CAPS[capabilities->NumberInputValueCaps];

  // Input value capabilities
  capabilities_length = capabilities->NumberInputValueCaps;
  HidP_GetValueCaps(HidP_Input, value_capabilities, &capabilities_length,
                    preparsed_data);

  unsigned long usage_length = num_buttons;
  PUSAGE usage = new USAGE[usage_length];
  long ret = HidP_GetUsages(HidP_Input, button_capabilities->UsagePage, 0,
                            usage, &usage_length, preparsed_data,
                            reinterpret_cast<PCHAR>(buffer->data.hid.bRawData),
                            buffer->data.hid.dwSizeHid);
  if (usage) {
    // commands_.at(0)->execute();
  }

  CloseHandle(hid_handle);
  delete[] usage;
  delete[] value_capabilities;
  delete[] button_capabilities;
  delete[] name;
  std::free(preparsed_data);
  delete[] capabilities;
  delete[] buffer;
  return 1;
}

int G27InputDriver::PollState() {
  // If device is connected, obtain handle to device

  GUID usb_class_guid = GUID_DEVINTERFACE_USB_DEVICE;
  HDEVINFO di_handle = SetupDiGetClassDevs(
      &usb_class_guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
  if (di_handle == INVALID_HANDLE_VALUE) return false;

  int device_index = 0;

  // Loop through all usb device interfaces until g27 is found
  while (true) {
    SP_DEVICE_INTERFACE_DATA di_data = {0};
    di_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    SetupDiEnumDeviceInterfaces(di_handle, NULL, &usb_class_guid, device_index,
                                &di_data);
    if (GetLastError() == ERROR_NO_MORE_ITEMS) break;

    SP_DEVINFO_DATA devinfo_data = {0};
    devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
    DWORD required_size = 0;

    SetupDiGetDeviceInterfaceDetail(di_handle, &di_data, NULL, NULL,
                                    &required_size, &devinfo_data);

    PSP_DEVICE_INTERFACE_DETAIL_DATA di_detail_data =
        new SP_DEVICE_INTERFACE_DETAIL_DATA[required_size];
    di_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

    SetupDiGetDeviceInterfaceDetail(di_handle, &di_data, di_detail_data,
                                    required_size, &required_size,
                                    &devinfo_data);

    HANDLE device_handle =
        CreateFile(di_detail_data->DevicePath, GENERIC_READ | GENERIC_WRITE, 0,
                   NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (device_handle == INVALID_HANDLE_VALUE) {
      delete[] di_detail_data;
      device_index++;
      continue;
    }

    WINUSB_INTERFACE_HANDLE usb_handle = {0};
    PUCHAR device_descriptor = new UCHAR[sizeof(USB_DEVICE_DESCRIPTOR)];
    ULONG length_transferred = 0;

    WinUsb_Initialize(device_handle, &usb_handle);
    WinUsb_GetDescriptor(usb_handle, URB_FUNCTION_GET_DESCRIPTOR_FROM_DEVICE, 0,
                         0x0409, device_descriptor, sizeof(device_descriptor),
                         &length_transferred);

    WinUsb_Free(usb_handle);
    delete[] di_detail_data;
    delete[] device_descriptor;
    device_index++;
  }
  return 0;
}

}  // namespace g27
}  // namespace hid
}  // namespace ecu
