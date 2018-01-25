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

#include "ui/mainwindow.h"
#include "ui_mainwindow.h"
#include "ui/ffb_widget.h"

#include <QMessageBox>

#include <ECU/ECU-version.h>

Mainwindow::Mainwindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  FFBWidget* ffb = new FFBWidget(this);
  ui->setupUi(this);
  this->setCentralWidget(ffb);
}

Mainwindow::~Mainwindow() { delete ui; }
void Mainwindow::on_actionAbout_triggered() {
  QMessageBox msg_box;
  msg_box.resize(500, 300);
  msg_box.setWindowTitle(tr("About ECU"));
  msg_box.setText(ECU_NAME_VERSION "\nAuthors:");
  msg_box.exec();
}
