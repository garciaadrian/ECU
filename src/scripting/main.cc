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

#include "lua.hpp"

namespace ecu {
namespace scripting {

int main() {
  lua_State* state = luaL_newstate();
  luaL_openlibs(state);

  int result = 0;

  
  lua_close(state);
  
  return 1;
}

}  // namespace scripting
}  // namespace ecu
