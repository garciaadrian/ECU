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

#include "scripting/callbacks.h"

namespace ecu {
namespace vm {

static int GetDriverIdx(lua_State* L) {
  const char *driver = luaL_checkstring(L, 1);

  lua_newtable(L);
  lua_pushinteger(L, 1);
  lua_pushstring(L, "Adrian Garcia Cruz");
  lua_settable(L, -3);

  return 1;
}

int SetERSLevel(lua_State* L) {
  double arg = luaL_checknumber(L, 1);
  // change ERS level. If success then return 1 else return 0
  lua_pushnumber(L, 1); // push result of this func
  
  return 1; // number of results
}

} // namespace vm
} // namespace ecu
