/**
*******************************************************************************
*                                                                             *
* ECU: iRacing MP4-30 Performance Analysis Project                            *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*******************************************************************************
*/

#include "ui/window.h"

namespace ecu {
namespace ui {

Window::Window(std::wstring title) : title_(title) {
  
}

Window::~Window() {
  CloseHandle(hwnd_);
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT message,
                                 WPARAM wParam, LPARAM lParam) {
  return DefWindowProc(hWnd, message, wParam, lParam);
}

bool Window::Initialize() {
  HINSTANCE hInstance = GetModuleHandle(nullptr);
  
  static bool has_registered_class = false;
  if (!has_registered_class) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = Window::WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;  // use the rc file!
    wcex.hIconSm = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"ECUWindowClass";
    
    if (!RegisterClassEx(&wcex))
      return false;
    has_registered_class = true;
  }

  DWORD window_style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
  DWORD window_ex_style = WS_EX_APPWINDOW | WS_EX_CONTROLPARENT;

  hwnd_ = CreateWindowEx(window_ex_style, L"ECUWindowClass", title_.c_str(),
                         window_style, CW_USEDEFAULT, CW_USEDEFAULT,
                         500, 500, nullptr, nullptr, hInstance, this);
  if (!hwnd_) {
    return false;
  }

  ShowWindow(hwnd_, SW_SHOWNORMAL);
  UpdateWindow(hwnd_);
  
}

} // namespace ui
} // namespace ecu
