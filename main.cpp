#include "git_version.h"
#include "mainwindow.h"
#include "shotwin.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSettings>

#include "initstaticplugins.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationDomain("http://github.com/Osse/shotwin");
    QCoreApplication::setOrganizationName("Øystein Walle");
    QCoreApplication::setApplicationName("Shotwin");
    QCoreApplication::setApplicationVersion(GIT_VERSION);
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Simple Qt-based view onto a Shotwell database."));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({{{"d", "database"}, QObject::tr("Location of Shotwell database."), "database"},
                       {{"m", "add-map"}, QObject::tr("Add a picture path mapping."), "mapping"},
                       {{"n", "no-config"}, QObject::tr("Don't update config file.")},
                       {{"N", "no-use-config"}, QObject::tr("Don't read existing config file.")}});

    parser.process(a);

    Args args;

    if (parser.isSet("database"))
        args.database = parser.value("database");

    if (parser.isSet("add-map"))
        args.mapList = parser.values("add-map");

    args.readConfig = !parser.isSet("no-config");
    args.updateConfig = !parser.isSet("no-use-config");

    Shotwin s;

    MainWindow w(&s);
    w.handleArgs(args);
    w.show();

    return a.exec();
}
