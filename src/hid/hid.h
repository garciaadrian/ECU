#ifndef HID_H
#define HID_H

#include <wtypes.h>

void hid_poll(LPARAM lParam);
void enumerate_devices();
void register_devices(HWND hWnd);

#endif
