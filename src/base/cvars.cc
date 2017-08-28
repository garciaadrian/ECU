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

#include "base/cvars.h"

namespace ecu {

CVar::CVar(std::string name, std::string description, float value,
       float default_value, int flags)
    : name_(name), description_(description), value_(value),
      default_value_(default_value), flags_(flags) {
  
}
CVar::~CVar() {}

void CVar::SetValue(const float value) {
  value_ = value;
}

CVarSystem::CVarSystem() {}
CVarSystem::~CVarSystem() {}

const CVar CVarSystem::GetCVar(const std::string& name) {
  for (auto& var : cvars_) {
    if (var.name() == name) {
      return var;
    }
  }
}

void CVarSystem::SetCVar(const std::string& name, const float value) {
  for (auto& var : cvars_) {
    if (name == var.name()) {
      var.SetValue(value);
    }
  }
}

void CVarSystem::AddCVar(CVar& var) {
  cvars_.push_back(var);
}

void MoveCVars(std::vector<std::unique_ptr<ecu::CVar>> to,
               std::vector<std::unique_ptr<ecu::CVar>> from) {
  for (auto& cvar : from) {
    to.push_back(std::move(cvar));
  }
}


}  // namespace ecu
