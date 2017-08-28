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

#include "car/iracing_input.h"

namespace ecu {
namespace iracing {

void to_json(nlohmann::json& j, const iRacingInput& input) {
  j = nlohmann::json{{"id_", input.id_}, {"text_", input.text_}};
}

void from_json(const nlohmann::json& j, iRacingInput& input) {
  input.id_ = j.at("id_").get<int>();
  input.text_ = j.at("text_").get<std::string>();
}


}  // namespace iracing
}  // namespaceecu
