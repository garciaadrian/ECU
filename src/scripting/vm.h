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

namespace ecu {
namespace vm {

class LuaVM {
 public:
  LuaVM();
  ~LuaVM();

  void LoadFile(const std::string& file);
  void StackDump();
  
  int GetGlobalInt(const std::string& var);

  void RegisterGlobal(int (*func)(lua_State* L), const std::string& func_name);
  
  lua_State* get_state() const { return state_; }
  
 private:
  lua_State* state_;

  void RegisterGlobals();
};
  
} // namespace vm
} // namespace ecu

#endif // SCRIPTING_VM_H_
