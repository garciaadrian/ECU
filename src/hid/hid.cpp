#include "hid.h"
#include "base/debug.h"
#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>
#include <wchar.h>

wchar_t debug[4096];

device g27;

LPOLESTR guid_product;
LPOLESTR guid_instance;

_GUID Instance;
_GUID Product;

BOOL CALLBACK DIEnumDeviceObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi,
                                          LPVOID pvRef) {
  wchar_t szName[260];

  HRESULT hr = StringCchCopy(szName, MAX_PATH, lpddoi->tszName);
  if (SUCCEEDED(hr)) {
    swprintf_s(debug, L"DEVICE OBJECT: %ls\n", lpddoi->tszName);
    OutputDebugString(debug);
  } else {
    DEXIT_PROCESS(L"Failed to obtain hid device");
  }

  return DIENUM_CONTINUE;
}

BOOL __stdcall DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
  StringFromCLSID(lpddi->guidProduct, &guid_product);
  StringFromCLSID(lpddi->guidInstance, &guid_instance);

  Instance = lpddi->guidInstance;
  Product = lpddi->guidProduct;

  swprintf_s(debug, L"\n #%llu: %ls (%ls)\n \
				GUID product: %ls \n \
				GUID instance: %ls  \n",
             (DWORDLONG)pvRef, lpddi->tszProductName, lpddi->tszInstanceName,
             guid_product, guid_instance);
  OutputDebugString((LPCWSTR)debug);
  return DIENUM_CONTINUE;
}

device initialize_hid(HINSTANCE hInstance, HWND hWnd) {
  if (DI_OK != DirectInput8Create(hInstance, DIRECTINPUT_VERSION,
                                  IID_IDirectInput8W, (LPVOID*)&g27.DInterface,
                                  NULL)) {
    DEXIT_PROCESS(L"DirectoryInput8Create failed.");
  }

  if (DI_OK !=
      g27.DInterface->EnumDevices(DI8DEVCLASS_GAMECTRL, DIEnumDevicesCallback,
                                  (void*)1, DIEDFL_FORCEFEEDBACK)) {
    DEXIT_PROCESS(L"EnumDevices failed.");
  }

  if (DI_OK !=
      g27.DInterface->CreateDevice(Instance, &g27.device_interface, NULL)) {
    DEXIT_PROCESS(L"CreateDevice failed.");
  }

  g27.device_interface->SetDataFormat(&c_dfDIJoystick);
  g27.device_interface->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE);

  g27.DIMouseCaps.dwSize = sizeof(DIDEVCAPS);
  HRESULT hr = g27.device_interface->GetCapabilities(&g27.DIMouseCaps);
  g27.device_available = ((g27.DIMouseCaps.dwFlags && DIDC_ATTACHED &&
                           (g27.DIMouseCaps.dwAxes > 2)));

  g27.device_interface->EnumObjects(DIEnumDeviceObjectsCallback, NULL,
                                    DIDFT_ALL);

  g27.dipdw.diph.dwSize = sizeof(DIPROPDWORD);
  g27.dipdw.diph.dwHeaderSize = sizeof(DIPROPDWORD);
  g27.dipdw.diph.dwObj = 0;
  g27.dipdw.diph.dwHow = DIPH_DEVICE;
  g27.dipdw.dwData = 10;

  if (FAILED(g27.device_interface->Acquire())) {
    DEXIT_PROCESS(L"Failed to acquire HID interface")
  }

  hr = g27.device_interface->SetProperty(Instance, &g27.dipdw.diph);

  // we need to return g27 because ecu-core needs the Release() function before
  // exiting the program
  return g27;
}
