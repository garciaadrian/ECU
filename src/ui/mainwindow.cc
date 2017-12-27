#include "ui/mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

#include <ECU/ECU-version.h>

Mainwindow::Mainwindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

Mainwindow::~Mainwindow() { delete ui; }
void Mainwindow::on_actionAbout_triggered() {
  QMessageBox msg_box;
  msg_box.setWindowTitle("About ECU");
  msg_box.setText(ECU_NAME_VERSION "\nAuthors:");
  msg_box.exec();
}
