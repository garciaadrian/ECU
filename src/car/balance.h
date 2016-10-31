//
#ifndef BALANCE_H
#define BALANCE_H

#include <Windows.h>
#include <libs/irsdk/irsdk_client.h>

static irsdkCVar BALANCE_Bias("dcBrakeBias");
static irsdkCVar BALANCE_LapDist("LapDist");

// Brake balance isn't concerned with the amount of turns, but the amount of
// braking zones.
// Create a calibration tool to detect the number of braking zones.

#define OKAYAMA 12

typedef struct Brake {
  float baseline = 56.0f;
  float bias = 0.0f;
  float braking_points[OKAYAMA] = {100,  400,  1000, 1900, 2138,
                                   2285, 2570, 2759, 3130}; // meters
  float balance_points[OKAYAMA] = {56.0,  55.5, 56.75, 57.00, 55.0,
                                   54.75, 57.0, 55.0,  56.0}; // std::vector?

  INPUT ip = {0}; // i key increases balance by 0.25;
  INPUT sp = {0}; // s key decrease balance by 0.25;

  void change_bias();
} Brake;

#endif
