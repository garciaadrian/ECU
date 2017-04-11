/**
*******************************************************************************
*                                                                             *
* ECU: iRacing MP4-30 Performance Analysis Project                            *
*                                                                             *
*******************************************************************************
* Copyright 2016 Adrian Garcia Cruz. All rights reserved.                     *
* Released under the BSD license. see LICENSE for more information            *
*                                                                             *
* Author: Adrian Garcia Cruz <garcia.adrian.cruz@gmail.com>                   *
*******************************************************************************
*/

#include <base/debug.h>
#include <limits>
#include <car/ers.h>

void ers_norm_rate(ECU *settings)
{
  float target = 6.0f;
  
  if (target > settings->dcMGUKDeployFixed) {
    float steps = target - settings->dcMGUKDeployFixed;
    input_send(steps, 0x58, settings);  /* 0x43 is 'X' key */
  }
  else if (target < settings->dcMGUKDeployFixed) {
    float steps = (target - settings->dcMGUKDeployFixed) * -1;
    input_send(steps, 0x43, settings);  /* 0x58 is 'C' key */
  }
}

void ers_derate(ECU *settings)
{
  float target = 12.0f;
  if (target > settings->dcMGUKDeployFixed) {
    float steps = target - settings->dcMGUKDeployFixed;
    input_send(steps, 0x58, settings);  /* 0x43 is 'X' key */
  }
  else if (target < settings->dcMGUKDeployFixed) {
    float steps = (target - settings->dcMGUKDeployFixed) * -1;
    input_send(steps, 0x43, settings);  /* 0x58 is 'C' key */
  }
}

void ers(ECU *settings)
{
  /* ERS de-rate */
  if (settings->SteeringWheelAngle > settings->max_angle ||
      (settings->SteeringWheelAngle) < (settings->max_angle * -1)) {
    ers_derate(settings);
  }
  else {
    /* LOGF(WARNING, "ERS normal rating"); */
    ers_norm_rate(settings);
  }
}
