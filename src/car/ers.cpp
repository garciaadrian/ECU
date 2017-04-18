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
#include <json.hpp>

using json = nlohmann::json;

#define DEFAULT_RATE 6.0
#define DERATE 14.0

#define RADIAN_DEGREE 57.2958

void ers_norm_rate(float normrate_fixed, float currentdeploy_fixed)
{
  if (normrate_fixed > currentdeploy_fixed) {
    float steps = normrate_fixed - currentdeploy_fixed;
    input_send(steps, 0x58);  // 0x43 is 'X' key
  }
  else if (normrate_fixed < currentdeploy_fixed) {
    float steps = (normrate_fixed - currentdeploy_fixed) * -1;
    input_send(steps, 0x43);  // 0x58 is 'C' key
  }
}

void ers_derate(float derate_fixed, float currentdeploy_fixed)
{
  if (derate_fixed > currentdeploy_fixed) {
    float steps = derate_fixed - currentdeploy_fixed;
    input_send(steps, 0x58);  // 0x43 is 'X' key
  }
  else if (derate_fixed < currentdeploy_fixed) {
    float steps = (derate_fixed - currentdeploy_fixed) * -1;
    input_send(steps, 0x43);  // 0x58 is 'C' key
  }
}

void ers(ECU *settings)
{
  float angle = 0;
  float mguk = 0;
  
  get_var(settings, "SteeringWheelAngle", &angle);
  get_var(settings, "dcMGUKDeployFixed", &mguk);
  
  angle = angle * RADIAN_DEGREE;
  
  json j = {
    {"type", "telemetry"},
    {"SteeringWheelAngle", angle}
  };
  
  send_json(j.dump());
  
  // TODO: validate normrate and derate for correct values
  if (angle > settings->config->angle ||
      (angle) < (settings->config->angle * -1)) {
    ers_derate(settings->config->derate_fixed, mguk);
  }
  else {
    ers_norm_rate(settings->config->normrate_fixed, mguk);
  }
}
