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

#ifndef UI_UI_EVENT_H_
#define UI_UI_EVENT_H_

#include <stdint.h>
#include <windows.h>

namespace ecu {
namespace ui {

class Window;

class UIEvent {
 public:
  explicit UIEvent(Window* target = nullptr) : target_(target) {}
  virtual ~UIEvent() = default;

  Window* target() const { return target_; }

 private:
  Window* target_ = nullptr;
};

class FileDropEvent : public UIEvent {
 public:
  FileDropEvent(Window* target, wchar_t* filename)
      : UIEvent(target), filename_(filename) {}
  ~FileDropEvent() override = default;

  wchar_t* filename() const { return filename_; }

 private:
  wchar_t* filename_ = nullptr;
};

class KeyEvent : public UIEvent {
 public:
  KeyEvent(Window* target, int key_code, int repeat_count, bool prev_state)
      : UIEvent(target),
        key_code_(key_code),
        repeat_count_(repeat_count),
        prev_state_(prev_state) {}
  ~KeyEvent() override = default;

  bool is_handled() const { return handled_; }
  void set_handled(bool value) { handled_ = value; }

  int key_code() const { return key_code_; }

  int repeat_count() const { return repeat_count_; }
  bool prev_state() const { return prev_state_; }

 private:
  bool handled_ = false;
  int key_code_ = 0;

  int repeat_count_ = 0;
  bool prev_state_ = false;  // Key previously down(true) or up(false)
};

class MouseEvent : public UIEvent {
 public:
  enum class Button {
    kNone = 0,
    kLeft,
    kRight,
    kMiddle,
    kX1,
    kX2,
  };

 public:
  MouseEvent(Window* target, Button button, int32_t x, int32_t y,
             int32_t dx = 0, int32_t dy = 0)
      : UIEvent(target), button_(button), x_(x), y_(y), dx_(dx), dy_(dy) {}
  ~MouseEvent() override = default;

  bool is_handled() const { return handled_; }
  void set_handled(bool value) { handled_ = value; }

  Button button() const { return button_; }
  int32_t x() const { return x_; }
  int32_t y() const { return y_; }
  int32_t z() const { return z_; }
  int32_t dx() const { return dx_; }
  int32_t dy() const { return dy_; }

 private:
  bool handled_ = false;
  Button button_;
  int32_t x_ = 0;
  int32_t y_ = 0;
  int32_t z_ = 0;
  int32_t dx_ = 0;
  int32_t dy_ = 0;
};

class RawInputEvent : public UIEvent {
 public:
  RawInputEvent(Window* target, LPARAM lParam)
      : UIEvent(target), lParam_(lParam) {}
  ~RawInputEvent() override = default;

  LPARAM lparam() const { return lParam_; }
  RAWINPUT rawinput() const { return raw_input_; }

 private:
  RAWINPUT raw_input_ = {0};
  LPARAM lParam_ = 0;
};

}  // namespace ui
}  // namespace ecu

#endif  // UI_UI_EVENT_H_
