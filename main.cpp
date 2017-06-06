#include "mainwindow.h"
#include "shotwin.h"

#include <QApplication>
#include <QSettings>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("http://github.com/Osse/shotwin");
    QCoreApplication::setOrganizationName("Øystein Walle");
    QCoreApplication::setApplicationName("Shotwin");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    Shotwin s;

    MainWindow w(&s);
    w.show();

    return a.exec();
}
