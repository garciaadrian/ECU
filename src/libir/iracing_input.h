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

#ifndef LIBIR_IRACING_INPUT_H_
#define LIBIR_IRACING_INPUT_H_

#include <string>

#include "json.hpp"

namespace iracing {

struct iRacingInput {
  int id_;
  std::string text_;
};

void to_json(nlohmann::json& j, const iRacingInput& input);
void from_json(const nlohmann::json& j, iRacingInput& input);

static const int kBrakeBiasInc = 1;
static const int kBrakeBiasDec = 2;
static const int kERSModeToggle = 3;

const iRacingInput BRAKE_BIAS_INC {kBrakeBiasInc, "Brake Bias Inc"};
const iRacingInput BRAKE_BIAS_DEC {kBrakeBiasDec, "Brake Bias Dec"};
const iRacingInput ERS_MODE_TOGGLE {kERSModeToggle, "ERS Mode Toggle"};

}  // namespace iracing

#endif // LIBIR_IRACING_INPUT_H_
