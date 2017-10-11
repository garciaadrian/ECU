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

#ifndef SCRIPTING_VM_H_
#define SCRIPTING_VM_H_

#include <string>

#include "lua.hpp"

class LuaVM {
 public:
  LuaVM();
  ~LuaVM();

  void LoadFile(const std::string& file);

 private:
  lua_State* state_;
};

#endif // SCRIPTING_VM_H_
