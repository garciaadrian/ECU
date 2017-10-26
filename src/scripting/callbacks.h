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

#ifndef SCRIPTING_CALLBACKS_H_
#define SCRIPTING_CALLBACKS_H_

#include "lua.hpp"

namespace ecu {
namespace vm {

static int GetDriverIdx(lua_State* L);

int SetERSLevel(lua_State* L);


} // namespace vm
} // namespace ecu

#endif // SCRIPTING_CALLBACKS_H_
