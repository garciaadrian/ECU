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

#ifndef BASE_COMMAND_H_
#define BASE_COMMAND_H_

namespace ecu {
namespace base {

class Command {
 public:
  virtual ~Command() {}
  virtual void execute() = 0;
};

class NullCommand : Command {
  void execute() override {}
};

}  // namespace base
}  // namespace ecu

#endif  // BASE_COMMAND_H_
