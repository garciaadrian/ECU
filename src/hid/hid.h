//
#ifndef HID_H
#define HID_H

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

typedef struct device {
  LPDIRECTINPUTDEVICE wheel;
  IDirectInput8* DInterface;
  IDirectInputDevice8* device_interface;
  DIDEVCAPS DIMouseCaps;
  BOOLEAN device_available;
  DIPROPDWORD dipdw;
  DIJOYSTATE joy;
  DIDEVICEOBJECTDATA joy_data[32];
  DWORD dw_items = 32;
} device;

device initialize_hid(HINSTANCE hInstance, HWND hWnd);

#endif