//
#include "ers.h"
#include <base/debug.h>
#include <limits>

// If the program has no data on the driver's avg braking point for different
// braking zones then the calibration must be ran and a few laps must be run

void ers::ers_generation()
{
  joules = 0.0f;
  
  QueryPerformanceFrequency(&frequency);
  QueryPerformanceCounter(&begin);

  // do {
  //   if (irsdkClient::instance().waitForData(16)) {
  //     if (brake >= 0.50f && throttle == 0.0f && speed >= 30.0f) {
  //       joules += 1920;
  //     } else if (brake <= 0.50f && throttle == 0.0f && speed >= 30.0f) {
  //       joules += (brake * 100) * 38.4f;
  //     }
  //   }

  // } while (g_Brake.getFloat() >
  //          0.0f); // if this doesn't work maybe replace with brake?
   QueryPerformanceCounter(&end);

  // if (throttle == 0.0f && brake >= 0.50f) {
  //   joules += 1920;
  // } else if (throttle == 0.0f && brake <= 0.50f) {
  //   joules += (brake * 100) * 38.4f;
  // }



  // elapsed_time =
  //     (end.QuadPart - begin.QuadPart) * 1000.0 / frequency.QuadPart;

  // DEBUG_OUT("ELASPED TIME IN BRAKING ZONE: %f\nJOULES GENERATED: %f MGU-K "
  //           "GENERATED: %f%%\n",
  //           elapsed_time, joules, (joules / 4000000.0f) * 100);
}
