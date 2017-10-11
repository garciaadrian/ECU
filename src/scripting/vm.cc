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
#include "g3log/g3log.hpp"
#include <fstream>

LuaVM::LuaVM() {
  state_ = luaL_newstate();
  luaL_openlibs(state_);
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
  if (error) {
    LOGF(g3::DEBUG, "%s\n", lua_tostring(state_, -1));
    lua_pop(state_, 1);
  }
  
  
}
