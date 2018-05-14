#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui
{
class MainWindow;
}

class Shotwin;
class QTreeView;

struct Args {
    QString database;
    QStringList mapList;
    bool updateConfig = true;
    bool readConfig = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Shotwin* shotwin, QWidget* parent = 0);
    ~MainWindow();

    void handleArgs(const Args& args);

    void showSettings2();

private:
    void openDatabase();
    void openDataBaseConnection(const QString& dbName);
    void initModelsAndViews();
    void setupTree(QTreeView* tree);
    void showTreeMenu(QTreeView* tree);
    void expandSingleNodes(QTreeView* tree);
    void aboutShotwin();
    void aboutQt();
    void showSettings();

private:
    Ui::MainWindow* ui;
    Shotwin* shotwin;
};

#endif
