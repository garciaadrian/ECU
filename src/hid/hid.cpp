#include "hid.h"
#include "base/debug.h"
#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>
#include <wchar.h>
#include <Hidsdi.h>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>

#define KEY_LENGTH 255

static unsigned short device_id[][2] = {
  {0x046D, 0xC29B} /* Logitech G27 */
};

static unsigned short keys[KEY_LENGTH] = {0};
static unsigned short keys_index = 0;

void hid_poll(LPARAM lParam)
{
  unsigned int buffer_size;
      
  GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &buffer_size,
                  sizeof(RAWINPUTHEADER));

  RAWINPUT *buffer = (RAWINPUT*)malloc(buffer_size);
      
  GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &buffer_size,
                  sizeof(RAWINPUTHEADER));
      
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_PREPARSEDDATA,
                        NULL, &buffer_size);
      
       
  PHIDP_PREPARSED_DATA preparsed_data = (PHIDP_PREPARSED_DATA)malloc(buffer_size);
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_PREPARSEDDATA,
                        preparsed_data, &buffer_size);
  
  PHIDP_CAPS caps = (PHIDP_CAPS)malloc(sizeof(HIDP_CAPS));

  HidP_GetCaps(preparsed_data, caps);

  GUID device_guid;
  HidD_GetHidGuid(&device_guid);
  
  wchar_t product_string[sizeof(wchar_t) * 128] = {0};
  wchar_t manufacturer_string[sizeof(wchar_t) * 128] = {0};
  wchar_t serial_number[sizeof(wchar_t) * 128] = {0};

  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_DEVICENAME,
                        NULL, &buffer_size);

  wchar_t *name = (wchar_t*)malloc(sizeof(wchar_t) * (buffer_size+1));
  
  GetRawInputDeviceInfo(buffer->header.hDevice, RIDI_DEVICENAME,
                        name, &buffer_size);
  
  HANDLE hid = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                          NULL, OPEN_EXISTING, NULL, NULL);
  BOOL h = true;
  
  if (hid != INVALID_HANDLE_VALUE) {
    h = HidD_GetProductString(hid, product_string, sizeof(wchar_t)*128);
    h = HidD_GetManufacturerString(hid, manufacturer_string, sizeof(wchar_t)*128);
    h = HidD_GetSerialNumberString(hid, serial_number, sizeof(wchar_t)*128);
  }

  unsigned short caps_length = caps->NumberInputButtonCaps;
      
  PHIDP_BUTTON_CAPS button_caps =
      (PHIDP_BUTTON_CAPS)malloc(sizeof(HIDP_BUTTON_CAPS) * caps->NumberInputButtonCaps);
      
      
  HidP_GetButtonCaps(HidP_Input, button_caps, &caps_length, preparsed_data);
      
  unsigned char num_buttons =
      button_caps->Range.UsageMax - button_caps->Range.UsageMin + 1;
      
  PHIDP_VALUE_CAPS value_caps =
      (PHIDP_VALUE_CAPS)malloc(sizeof(HIDP_VALUE_CAPS) * caps->NumberInputValueCaps);

  caps_length = caps->NumberInputValueCaps;
  HidP_GetValueCaps(HidP_Input, value_caps, &caps_length, preparsed_data);

  unsigned long usage_length = num_buttons;
  PUSAGE usage = (PUSAGE)malloc(sizeof(USAGE) * usage_length);
  long ret;
  ret = HidP_GetUsages(HidP_Input, button_caps->UsagePage, 0, usage, &usage_length,
                       preparsed_data, (PCHAR)buffer->data.hid.bRawData,
                       buffer->data.hid.dwSizeHid);

  /* This can be improved drastically, iterate over keys first then usage */
  for (int i = 0; i < usage_length; i++) {
                
    bool present = false;
    for (int x = 0; x <= keys_index; x++) {
          
      if (keys[x] == usage[i]) {
        present = true;
        break;
      }
         
    }

    /* If not in key array */
    if (!present) {
      keys[keys_index] = usage[i];
      LOGF(DEBUG, "Button %d pressed (%S %S)",
           usage[i],
           (manufacturer_string[0] == '\0') ? L"*": manufacturer_string,
           (product_string[0] == '\0') ? L"*": product_string);
      keys_index++;
    }
  }

  /* Some key(s) was released */
  /* This block is assuming the smallest button value is 1 */
  if (keys_index > usage_length) {
    for (int i = 0; i <= KEY_LENGTH; i++) {
      bool present = false;
          
      for (int x = 0; x <= usage_length; x++) {
        if (keys[i] == usage[x]) {
          present = true;
          break;
        }
      }
      if (!present) {
        LOGF(DEBUG, "Button %d pressed (%S %S)",
           keys[i],
           (manufacturer_string[0] == '\0') ? L"*": manufacturer_string,
           (product_string[0] == '\0') ? L"*": product_string);
        keys[i] = 0;
        keys_index--;
      }
    }
  }
      
  unsigned long usage_value;
  for (int i = 0; i < caps->NumberInputValueCaps; i++) {
        
    HidP_GetUsageValue(HidP_Input, value_caps[i].UsagePage, 0,
                       value_caps[i].Range.UsageMin, &usage_value,
                       preparsed_data, (PCHAR)buffer->data.hid.bRawData,
                       buffer->data.hid.dwSizeHid);
    ret++;
  }
  
  free(buffer);
  free(preparsed_data);
  free(button_caps);
  free(usage);

}

void enumerate_devices()
{
  /* Check if there are recgonized devices plugged in */
  unsigned int num_devices = 0;
  LOGF(DEBUG, "Enumerating system HID devices (manufacturer product vId pId)");
  GetRawInputDeviceList(NULL, &num_devices, sizeof(RAWINPUTDEVICELIST));

  if (num_devices <= 0) {
    DEXIT_PROCESS(L"Failed to enumerate system devices", 0);
  }

  RAWINPUTDEVICELIST *devices =
      (RAWINPUTDEVICELIST*)malloc(num_devices * sizeof(RAWINPUTDEVICELIST));
  
  if (!GetRawInputDeviceList(devices, &num_devices, sizeof(RAWINPUTDEVICELIST))) {
    DEXIT_PROCESS(L"Failed to enumerate system devices", GetLastError());
  }

  for (int i = 0; i < num_devices; i++) {
    unsigned int size = 0;
    int ret = 0;

    ret = GetRawInputDeviceInfo(devices[i].hDevice, RIDI_DEVICENAME, NULL, &size);
    
    wchar_t *name = (wchar_t*)malloc(sizeof(wchar_t) * (size+1));
    
    ret = GetRawInputDeviceInfo(devices[i].hDevice, RIDI_DEVICENAME, name, &size);
    
    
    ret = GetRawInputDeviceInfo(devices[i].hDevice, RIDI_DEVICEINFO, NULL, &size);

    PRID_DEVICE_INFO device_info =
        (PRID_DEVICE_INFO)malloc(size);
    
    ret = GetRawInputDeviceInfo(devices[i].hDevice, RIDI_DEVICEINFO, device_info, &size);
    
    HANDLE hid_handle = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE,
                                   NULL, OPEN_ALWAYS,
                                   FILE_FLAG_OVERLAPPED, NULL);
    
    wchar_t product_string[sizeof(wchar_t) * 128] = {0};
    wchar_t manufacturer_string[sizeof(wchar_t) * 128] = {0};
    wchar_t serial_number[sizeof(wchar_t) * 128] = {0};

    if (hid_handle != INVALID_HANDLE_VALUE) {
      ret = HidD_GetManufacturerString(hid_handle, manufacturer_string, sizeof(wchar_t) * 128);
      ret = HidD_GetProductString(hid_handle, product_string, sizeof(wchar_t) * 128);
      ret = HidD_GetSerialNumberString(hid_handle, serial_number, sizeof(wchar_t) * 128);

      LOGF(INFO, "device %S %S %04x %04x",
           (manufacturer_string[0] == '\0') ? L"*": manufacturer_string,
           (product_string[0] == '\0') ? L"*": product_string,
           device_info->hid.dwVendorId,
           device_info->hid.dwProductId);
      
      CloseHandle(hid_handle);
    }
    
    free(device_info);
    free(name);
  }
  
  free(devices);
}

void register_devices(HWND hWnd)
{
  char debug[256];
  wchar_t debug_wide[256];
  
  RAWINPUTDEVICE wheel_input;

  LOGF(DEBUG, "This is a debug log from hid module");

  wheel_input.usUsagePage = 1;
  wheel_input.usUsage = 4;
  wheel_input.dwFlags = RIDEV_INPUTSINK;
  wheel_input.hwndTarget = hWnd;

  if (!RegisterRawInputDevices(&wheel_input, 1, sizeof(RAWINPUTDEVICE))) {
    LOGF(FATAL, "failed to register HID devices");
    DEXIT_PROCESS(L"Failed to register HID devices", GetLastError());
  }
  
}
