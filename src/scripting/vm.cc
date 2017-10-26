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

#include "scripting/vm.h"
#include "scripting/callbacks.h"

#include "g3log/g3log.hpp"

#include <fstream>

namespace ecu {
namespace vm {

LuaVM::LuaVM() {
  state_ = luaL_newstate();
  luaL_openlibs(state_);
  RegisterGlobals();
}

LuaVM::~LuaVM() {
  lua_close(state_);
}

void LuaVM::LoadFile(const std::string& file) {
  std::ifstream in(file, std::ios::in);

  if (!in) {
    return;
  }
  
  std::string contents;

  in.seekg(0, std::ios::end);
  contents.reserve(in.tellg());
  in.seekg(0, std::ios::beg);
  
  contents.assign((std::istreambuf_iterator<char>(in)),
                  std::istreambuf_iterator<char>());
  
  int error = luaL_loadstring(state_, contents.c_str());
  StackDump();
  lua_pcall(state_, 0, 0, 0);
  StackDump();
  
  if (error) {
    LOGF(g3::WARNING, "Unable to load 'default.lua'");
    LOGF(g3::FATAL, "%s\n", lua_tostring(state_, -1));
    lua_pop(state_, 1);
  }

  lua_getglobal(state_, "OnDriverPitExit");
  if (!lua_isfunction(state_, -1)) {
    LOGF(g3::WARNING, "OnDriverPitExit doesn't exist!");
  }
  
  lua_pushnumber(state_, 12);
  
  if (lua_pcall(state_, 1, 1, 0) != 0) {
    LOGF(g3::WARNING, "error running function OnDriverPitEnter. error %s",
         lua_tostring(state_, -1));
  }

  auto result = lua_tonumber(state_, -1);
  if (!result) {
    LOGF(g3::WARNING, "OnDriverPitEnter should return a number");
  }
  lua_pop(state_, 1);
}

void LuaVM::RegisterGlobal(int (*func)(lua_State* L), const std::string& func_name) {
  lua_pushcfunction(state_, func);
  lua_setglobal(state_, func_name.c_str());
}

void LuaVM::RegisterGlobals() {
  lua_pushcfunction(state_, SetERSLevel);
  lua_setglobal(state_, "SetERSLevel");
}

void LuaVM::StackDump() {
  LOGF(g3::DEBUG, " --- Begin Lua Stackdump --- ");
  int top = lua_gettop(state_);
  for (int i = 1; i <= top; i++) {
    int type = lua_type(state_, i);

    switch (type) {
    case LUA_TSTRING: {
      LOGF(g3::DEBUG, "'%s'", lua_tostring(state_, i));
      break;
    }
    case LUA_TBOOLEAN: {
      LOGF(g3::DEBUG, "%d", lua_toboolean(state_, i));
      break;
    }
    case LUA_TNUMBER: {
      LOGF(g3::DEBUG, "%g", lua_tonumber(state_, i));
      break;
    }
    default: {
      LOGF(g3::DEBUG, "Unknown lua type: '%s'", lua_typename(state_, type));
      break;
    }
    }
  }
  LOGF(g3::DEBUG, " --- End Lua Stackdump: --- ");
}

int LuaVM::GetGlobalInt(const std::string& var) {
  lua_getglobal(state_, var.c_str());

  int result = 0;
  result = (int)lua_tointeger(state_, -1);
    
  // Remove result from the stack
  lua_pop(state_, 1);
  return result;
}

} // namespace vm
} // namespace ecu
