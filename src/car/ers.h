//
#ifndef ERS_H
#define ERS_H // it all stated hiar

#include <Windows.h>
#include <libs/irsdk/irsdk_client.h>

struct ers {
  LARGE_INTEGER frequency; // ticks per second
  LARGE_INTEGER begin, end;
  double elapsed_time; // in ms

  float joules = 0.0f;

  // Check if MGU-K is generating energy
  void ers_generation();
};

#endif // ERS_H
