#include "mainwindow.h"
#include <QApplication>

#include <QSettings>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("http://github.com/Osse/shotwin");
    QCoreApplication::setOrganizationName("Øystein Walle");
    QCoreApplication::setApplicationName("Shotwin");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    MainWindow w;
    w.show();

    return a.exec();
}
