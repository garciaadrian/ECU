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

#ifndef UI_FFBMANAGER_H_
#define UI_FFBMANAGER_H_

#include <QThread>
#include <QMutex>
#include <QWaitCondition>

#include <memory>

namespace ecu {
namespace joy {

class DeviceManager;

}  // namespace joy
}  // namespace ecu

class ForcefeedbackThread : public QThread {
  Q_OBJECT

 public:
  explicit ForcefeedbackThread(QObject* parent = nullptr);
  ~ForcefeedbackThread() override;

 protected:
  void run() override;

 private:
  QMutex mutex_;
  QWaitCondition condition_;
};

#endif  // UI_FFBMANAGER_H_
