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

#include "ui/ffb_widget.h"
#include "ui_ffb_widget.h"

#include "ffb/di_manager.h"

#include <QMessageBox>

FFBWidget::FFBWidget(QWidget* parent) : QWidget(parent), ui(new Ui::FFBWidget) {
  HWND window = reinterpret_cast<HWND>(parent->winId());
  device_manager_ = std::make_unique<ecu::joy::DeviceManager>(window);

  ui->setupUi(this);
  QGridLayout* layout = new QGridLayout;
  QLabel* label1 = new QLabel("1", this);
  QLabel* label2 = new QLabel("2", this);
  ui->gridLayout->addWidget(label1, 1, 0, 1, 1);
  ui->gridLayout->addWidget(label2, 1, 1, 1, 1);
  connect(device_manager_.get(), &ecu::joy::DeviceManager::DeviceConnected,
          this, &FFBWidget::AddDevice);
  connect(device_manager_.get(), &ecu::joy::DeviceManager::DeviceDisconnected,
          this, &FFBWidget::RemoveDevice);
  device_manager_->Enumerate();
}

void FFBWidget::AddDevice(const QString& device) {
  ui->comboBox->addItem(QIcon(), device, Qt::DisplayRole);
}
void FFBWidget::RemoveDevice(const QString& device) {
  int index = ui->comboBox->findText(device, Qt::MatchExactly);
  ui->comboBox->removeItem(index);
}

FFBWidget::~FFBWidget() { delete ui; }

void FFBWidget::SetSliderLablels() {}

void FFBWidget::on_comboBox_currentIndexChanged(const QString& device) {
  device_manager_->SetCurrentDevice(device);
}

void FFBWidget::on_horizontalSlider_sliderMoved(int position) {
  device_manager_->SetMaxForce(position);
}
