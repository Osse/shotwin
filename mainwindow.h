#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class Shotwin;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Shotwin* shotwin, QWidget* parent = 0);
    ~MainWindow();

private:
    void openDatabase();
    void openDataBaseConnection(const QString& dbName);
    void initModelsAndViews();
    void aboutShotwin();
    void aboutQt();
    void showSettings();

private:
    Ui::MainWindow* ui;
    Shotwin* shotwin;
};

#endif
