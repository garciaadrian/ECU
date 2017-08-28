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

#ifndef BASE_CVARS_H_
#define BASE_CVARS_H_

#include <string>
#include <vector>
#include <memory>

namespace ecu {

class CVar {
 public:
  CVar(std::string name, std::string description, float value,
       float default_value, int flags);
  ~CVar();

  const std::string& name() const { return name_; }
  const std::string& description() const { return description_; }
  float value() const { return value_; }
  float default_value() const { return default_value_; }
  int flags() const { return flags_; }

  void SetValue(const float value);

 private:
  std::string name_;
  std::string description_;
  float value_;
  float default_value_;
  int flags_;
};

class CVarSystem {
 public:
  CVarSystem();
  ~CVarSystem();

  const CVar GetCVar(const std::string& name);
  void SetCVar(const std::string& name, const float value);

  void AddCVar(CVar& var);
  
 private:
  std::vector<CVar> cvars_;
};

}  // namespace ecu

void MoveCVars(std::vector<std::unique_ptr<ecu::CVar>> to,
               std::vector<std::unique_ptr<ecu::CVar>> from);

#endif // BASE_CVARS_H_
