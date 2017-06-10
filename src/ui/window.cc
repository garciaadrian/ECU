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

Window::Window(std::wstring title) : title_(title) {}

Window::~Window() {
  if (hwnd_) {
    SetWindowLongPtr(hwnd_, GWLP_USERDATA, 0);
    CloseWindow(hwnd_);
    hwnd_ = nullptr;
  }
  if (icon_) {
    DestroyIcon(icon_);
    icon_ = nullptr;
  }
}

void Window::AttachListener(WindowListener* listener) {
  if (in_listener_loop_) {
    pending_listener_attaches_.push_back(listener);
    return;
  }
  auto it = std::find(listeners_.begin(), listeners_.end(), listener);
  if (it != listeners_.end()) {
    return;
  }
  listeners_.push_back(listener);
  Invalidate();
}

void Window::DetachListener(WindowListener* listener) {
  if (in_listener_loop_) {
    pending_listener_detaches_.push_back(listener);
    return;
  }
  auto it = std::find(listeners_.begin(), listeners_.end(), listener);
  if (it == listeners_.end()) {
    return;
  }
  listeners_.erase(it);
}

void Window::ForEachListener(std::function<void(WindowListener*)> fn) {
  _ASSERT(in_listener_loop_ == FALSE);
  in_listener_loop_ = true;
  for (auto listener : listeners_) {
    fn(listener);
  }
  in_listener_loop_ = false;
  while (!pending_listener_attaches_.empty()) {
    auto listener = pending_listener_attaches_.back();
    pending_listener_attaches_.pop_back();
    AttachListener(listener);
  }
  while (!pending_listener_detaches_.empty()) {
    auto listener = pending_listener_detaches_.back();
    pending_listener_detaches_.pop_back();
    DetachListener(listener);
  }
}

void Window::OnCloseDelegates() {
  UIEvent e(this);
  ForEachListener([&e](auto listener) { listener->OnClosing(&e); });
  on_closing(&e);
  ForEachListener([&e](auto listener) { listener->OnClosed(&e); });
  on_closed(&e);
}

void Window::OnClose() {
  if (!closing_ && hwnd_) {
    closing_ = true;
    CloseWindow(hwnd_);
  }

  // super::OnClose();
  OnCloseDelegates();
}

void Window::OnMainMenuChange() {
  auto main_menu = reinterpret_cast<Win32MenuItem*>(main_menu_.get());
  // Don't actually set the menu if we're fullscreen. We'll do that later.
  if (main_menu_ && !is_fullscreen()) {
    ::SetMenu(hwnd_, main_menu->handle());
  }
}

void Window::Invalidate() {}

void Window::Close() {
  if (closing_) {
    return;
  }
  closing_ = true;
  Close();
  OnClose();
  DestroyWindow(hwnd_);
}

LRESULT CALLBACK Window::WndProcThunk(HWND hwnd, UINT message, WPARAM wParam,
                                      LPARAM lParam) {
  Window* window = nullptr;
  if (message == WM_NCCREATE) {
    auto create_struct = reinterpret_cast<LPCREATESTRUCT>(lParam);
    window = reinterpret_cast<Window*>(create_struct->lpCreateParams);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (__int3264)(LONG_PTR)window);
  } else {
    window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
  }
  if (window) {
    return window->WndProc(hwnd, message, wParam, lParam);
  } else {
    return DefWindowProc(hwnd, message, wParam, lParam);
  }
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT message, WPARAM wParam,
                                 LPARAM lParam) {
  if (hwnd != hwnd_) {
    return DefWindowProc(hwnd, message, wParam, lParam);
  }

  switch (message) {
    case WM_NCCREATE:
      break;
    case WM_CREATE:
      break;
    case WM_DESTROY:
      // TODO(garciaadrian): call listeners
      break;
    case WM_CLOSE:
      closing_ = true;
      Close();
      OnClose();
      break;
    case WM_MOVING:
      break;
    case WM_MOVE:
      break;
    case WM_SIZING:
      break;
    case WM_SIZE: {
      // TODO(garciaadrian): call listeners
      break;
    }
    case WM_PAINT: {
      ValidateRect(hwnd_, nullptr);
      static bool in_paint = false;
      if (!in_paint) {
        in_paint = true;
        // TODO(garciaadrian): call listeners
        in_paint = false;
      }
      return 0;
    }
    case WM_ERASEBKGND:
      return 0;
    case WM_DISPLAYCHANGE:
      break;
    case WM_ACTIVATEAPP:
    case WM_SHOWWINDOW: {
      if (wParam = TRUE) {
        // TODO(garciaadrian): call listeners
      } else {
        // TODO(garciaadrian): call listeners
      }
    }
    case WM_KILLFOCUS: {
      has_focus_ = false;
      // TODO(garciaadrian): call listeners
      break;
    }
    case WM_SETFOCUS: {
      has_focus_ = true;
      // TODO(garciaadrian): call listeners
      break;
    }
    case WM_MENUCOMMAND: {
      MENUINFO menu_info = {0};
      menu_info.cbSize = sizeof(menu_info);
      menu_info.fMask = MIM_MENUDATA;
      GetMenuInfo(HMENU(lParam), &menu_info);
      auto parent_item = reinterpret_cast<Win32MenuItem*>(menu_info.dwMenuData);
      auto child_item =
          reinterpret_cast<Win32MenuItem*>(parent_item->child(wParam));
      _ASSERT(child_item != nullptr);
      break;
    }
  }

  return DefWindowProc(hwnd, message, wParam, lParam);
}

bool Window::Initialize() {
  HINSTANCE hInstance = GetModuleHandle(nullptr);

  static bool has_registered_class = false;
  if (!has_registered_class) {
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = Window::WndProcThunk;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;  // use the rc file!
    wcex.hIconSm = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"ECUWindowClass";

    if (!RegisterClassEx(&wcex)) return false;
    has_registered_class = true;
  }

  DWORD window_style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
  DWORD window_ex_style = WS_EX_APPWINDOW | WS_EX_CONTROLPARENT;
  RECT rc = {0, 0, width_, height_};
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

  hwnd_ = CreateWindowEx(window_ex_style, L"ECUWindowClass", title_.c_str(),
                         window_style, rc.left, rc.top, rc.right - rc.left,
                         rc.bottom - rc.top, nullptr, nullptr, hInstance, this);
  if (!hwnd_) {
    return false;
  }

  DragAcceptFiles(hwnd_, true);

  ShowWindow(hwnd_, SW_SHOWNORMAL);
  UpdateWindow(hwnd_);

  arrow_cursor_ = LoadCursor(nullptr, IDC_ARROW);

  // Initial state.
  if (!is_cursor_visible_) {
    ShowCursor(false);
  }
  if (has_focus_) {
    SetFocus(hwnd_);
  }
}

bool Window::set_title(const std::wstring& title) {
  SetWindowText(hwnd_, title.c_str());
  return true;
}

bool Window::is_fullscreen() const { return fullscreen_; }

void Window::Resize(int32_t width, int32_t height) {
  RECT rc = {0, 0, width, height};
  bool has_menu = !is_fullscreen() && (main_menu_ ? true : false);
  AdjustWindowRect(&rc, GetWindowLong(hwnd_, GWL_STYLE), has_menu);
  if (true) {
    rc.right += 100 - rc.left;
    rc.left = 100;
    rc.bottom += 100 - rc.top;
    rc.top = 100;
  }
  MoveWindow(hwnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
             TRUE);
}

void Window::Resize(int32_t left, int32_t top, int32_t right, int32_t bottom) {
  RECT rc = {left, top, right, bottom};
  bool has_menu = !is_fullscreen() && (main_menu_ ? true : false);
  AdjustWindowRect(&rc, GetWindowLong(hwnd_, GWL_STYLE), has_menu);
  MoveWindow(hwnd_, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
             TRUE);
}

std::unique_ptr<ui::MenuItem> MenuItem::Create(Type type,
                                               const std::wstring& text,
                                               const std::wstring& hotkey,
                                               std::function<void()> callback) {
  return std::make_unique<Win32MenuItem>(type, text, hotkey, callback);
}

Win32MenuItem::Win32MenuItem(Type type, const std::wstring& text,
                             const std::wstring& hotkey,
                             std::function<void()> callback)
    : MenuItem(type, text, hotkey, std::move(callback)) {
  switch (type) {
    case MenuItem::Type::kNormal:
      handle_ = CreateMenu();
      break;
    case MenuItem::Type::kPopup:
      handle_ = CreatePopupMenu();
      break;
    default:
      break;
  }

  if (handle_) {
    MENUINFO menu_info = {0};
    menu_info.cbSize = sizeof(menu_info);
    menu_info.fMask = MIM_MENUDATA | MIM_STYLE;
    menu_info.dwMenuData = ULONG_PTR(this);
    menu_info.dwStyle = MNS_NOTIFYBYPOS;
    SetMenuInfo(handle_, &menu_info);
  }
}

Win32MenuItem::~Win32MenuItem() {
  if (handle_) {
    DestroyMenu(handle_);
  }
}

void Win32MenuItem::OnChildAdded(MenuItem* generic_child_item) {
  auto child_item = static_cast<Win32MenuItem*>(generic_child_item);

  switch (child_item->type()) {
    case MenuItem::Type::kNormal:

      break;
    case MenuItem::Type::kPopup:
      AppendMenuW(handle_, MF_POPUP,
                  reinterpret_cast<UINT_PTR>(child_item->handle()),
                  child_item->text().c_str());
      break;
    case MenuItem::Type::kSeparator:
      AppendMenuW(handle_, MF_SEPARATOR, UINT_PTR(child_item->handle_), 0);
      break;
    case MenuItem::Type::kString:
      auto full_name = child_item->text();
      if (!child_item->hotkey().empty()) {
        full_name += L"\t" + child_item->hotkey();
      }
      AppendMenuW(handle_, MF_STRING, UINT_PTR(child_item->handle_),
                  full_name.c_str());
      break;
  }
}

void Win32MenuItem::OnChildRemoved(MenuItem* generic_child_item) {}

}  // namespace ui
}  // namespace ecu
