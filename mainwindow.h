#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

private:
    void openDatabase();
    void openDataBaseConnection(const QString& dbName);
    void initModelsAndViews();
    void aboutShotwin();
    void aboutQt();

private:
    Ui::MainWindow* ui;
};

#endif
