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

#ifndef UI_MAINWINDOW_H_
#define UI_MAINWINDOW_H_

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class FFBWidget;

class Mainwindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit Mainwindow(QWidget* parent = nullptr);
  ~Mainwindow() override;

 private slots:
  void on_actionAbout_triggered();

 private:
  Ui::MainWindow* ui;
  FFBWidget* ffb_widget;
};

#endif  // UI_MAINWINDOW_H_
