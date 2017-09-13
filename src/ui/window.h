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

#ifndef UI_WINDOW_H_
#define UI_WINDOW_H_

#include <Windows.h>
#include <string>
#include <shellapi.h>

#include "ui/menu_item.h"
#include "ui/window_listener.h"
#include "base/delegate.h"

namespace ecu {
namespace ui {

class Window {
 public:
  Window(std::wstring title);
  ~Window();

  bool Initialize();
  HWND hwnd() const { return hwnd_; }
  bool set_title(const std::wstring& title);
  bool is_fullscreen() const;

  void Close();

  void Resize(int32_t width, int32_t height);
  void Resize(int32_t left, int32_t top, int32_t right, int32_t bottom);

  void set_main_menu(std::unique_ptr<MenuItem> main_menu) {
    main_menu_ = std::move(main_menu);
    OnMainMenuChange();
  }

  static LRESULT CALLBACK WndProcThunk(HWND hWnd, UINT message, WPARAM wParam,
                                       LPARAM lParam);

  // Fucking CMake!
  virtual LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam,
                          LPARAM lParam);

  void AttachListener(WindowListener* listener);
  void DetachListener(WindowListener* listener);

  void Invalidate();

 public:
  Delegate<UIEvent*> on_closing;
  Delegate<UIEvent*> on_closed;
  Delegate<UIEvent*> on_quit;
  Delegate<RawInputEvent*> on_raw_input;

 protected:
  void ForEachListener(std::function<void(WindowListener*)> fn);
  void TryForEachListener(std::function<bool(WindowListener*)> fn);

  // All currently-attached listeners that get event notifications.
  bool in_listener_loop_ = false;
  std::vector<WindowListener*> listeners_;
  std::vector<WindowListener*> pending_listener_attaches_;
  std::vector<WindowListener*> pending_listener_detaches_;

  void OnMainMenuChange();
  void OnCloseDelegates();
  void OnRawInput(LPARAM lParam);
  void OnClose();

 private:
  HWND hwnd_ = nullptr;
  HICON icon_ = nullptr;
  HCURSOR cursor_ = nullptr;
  HCURSOR arrow_cursor_ = nullptr;
  bool closing_ = false;
  bool fullscreen_ = false;

  int32_t width_ = 0;
  int32_t height_ = 0;
  bool has_focus_ = true;
  bool is_cursor_visible_ = false;

  std::unique_ptr<MenuItem> main_menu_;
  std::wstring title_;
};

class Win32MenuItem : public MenuItem {
 public:
  Win32MenuItem(Type type, const std::wstring& text, const std::wstring& hotkey,
                std::function<void()> callback);
  ~Win32MenuItem() override;

  HMENU handle() { return handle_; }

  using MenuItem::OnSelected;

 protected:
  void OnChildAdded(MenuItem* child_item) override;
  void OnChildRemoved(MenuItem* child_item) override;

 private:
  HMENU handle_ = nullptr;
};

}  // namespace ui
}  // namespace ecu

#endif  // UI_WINDOW_H_
