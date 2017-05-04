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

#ifndef UI_WINDOW_H_
#define UI_WINDOW_H_

#include <Windows.h>
#include <string>

namespace ecu {
namespace ui {

class Window {
 public:
  Window(std::wstring title);
  ~Window();
  
  bool Initialize();
  HWND hwnd() const { return hwnd_; }
  bool set_title(const std::wstring& title);
  static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                                  LPARAM lParam);
 private:
  HWND hwnd_ = nullptr;
  HICON icon_ = nullptr;
  HCURSOR cursor_ = nullptr;
  std::wstring title_;
};

}  // namespace ui
}  // namespace ecu

#endif // UI_WINDOW_H_
