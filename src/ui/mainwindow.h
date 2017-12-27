#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class Mainwindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit Mainwindow(QWidget* parent = nullptr);
  ~Mainwindow() override;

 private slots:
  void on_actionAbout_triggered();

 private:
  Ui::MainWindow* ui;
};
