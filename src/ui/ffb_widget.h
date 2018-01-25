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

#ifndef UI_FFB_WIDGET_H_
#define UI_FFB_WIDGET_H_

#include <QWidget>

#include <memory>

namespace ecu {
namespace joy {
class DeviceManager;
}  // namespace joy
}  // namespace ecu

namespace Ui {
class FFBWidget;
}

class FFBWidget : public QWidget {
  Q_OBJECT

 public:
  explicit FFBWidget(QWidget* parent = nullptr);
  ~FFBWidget() override;

 public slots:
  void AddDevice(const QString& device);
  void RemoveDevice(const QString& device);

 private slots:
  void on_comboBox_currentIndexChanged(const QString& device);

  void on_horizontalSlider_sliderMoved(int position);

 private:
  Ui::FFBWidget* ui = nullptr;
  std::unique_ptr<ecu::joy::DeviceManager> device_manager_{};

  void SetSliderLablels();
};

#endif  // UI_FFB_WIDGET_H_
