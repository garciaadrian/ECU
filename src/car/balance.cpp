//
#include "balance.h"
#include <cstddef>

void Brake::change_bias() {
  // // move these somewhere
  // ip.type = INPUT_KEYBOARD;
  // sp.type = INPUT_KEYBOARD;
  // ip.ki.wVk = 0x49;
  // sp.ki.wVk = 0x53;

  // if (irsdkClient::instance().waitForData(16)) {
  //   bias = g_Bias.getFloat() + baseline;

  //   for (int idx = 0; idx < OKAYAMA; idx++) {
  //     if (g_LapDist.getFloat() < braking_points[idx]) {
  //       if (bias > balance_points[idx]) {
  //         while (bias != balance_points[idx]) {
  //           sp.ki.dwFlags = 0;
  //           SendInput(1, &sp, sizeof(INPUT));
  //           Sleep(50);

  //           sp.ki.dwFlags = KEYEVENTF_KEYUP;
  //           SendInput(1, &sp, sizeof(INPUT));

  //           bias -= 0.25f;
  //         }
  //       }

  //       else if (bias < balance_points[idx]) {
  //         while (bias != balance_points[0]) {
  //           ip.ki.dwFlags = 0;
  //           SendInput(1, &ip, sizeof(INPUT));
  //           Sleep(50);

  //           ip.ki.dwFlags = KEYEVENTF_KEYUP;
  //           SendInput(1, &ip, sizeof(INPUT));

  //           bias += 0.25f;
  //         }
  //       }
  //     }
  //   }
  // }
}
