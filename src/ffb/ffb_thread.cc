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

#include "ffb/ffb_thread.h"

#include "ffb/di_manager.h"

#include <ECU/ECU-version.h>

ForcefeedbackThread::ForcefeedbackThread(QObject* parent) : QThread(parent) {}

ForcefeedbackThread::~ForcefeedbackThread() {
  mutex_.lock();
  condition_.wakeOne();
  mutex_.unlock();

  wait();
}
