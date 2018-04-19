#include "git_version.h"
#include "mainwindow.h"
#include "shotwin.h"

#include "tree.hh"
#include <iostream>
#include <string>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QSettings>

#include "initstaticplugins.h"

using std::cout;
using std::endl;
using std::find;
using std::string;

template <class T, class tna = std::allocator<tree_node_<T> > >
class leaves
{
public:
    leaves()
    {
    }
    leaves(const tree<T, tna>& tree) : t(tree)
    {
    }

    typename tree<T, tna>::leaf_iterator begin() const;
    typename tree<T, tna>::leaf_iterator end() const;

private:
    const tree<T, tna>& t;
};

template <class T, class tla>
typename tree<T, tla>::leaf_iterator leaves<T, tla>::begin() const
{
    return t.begin_leaf();
}

template <class T, class tla>
typename tree<T, tla>::leaf_iterator leaves<T, tla>::end() const
{
    return t.end_leaf();
}

void treetest()
{
    tree<string> tree;

    auto top = tree.begin();

    auto newestElement = tree.insert(top, "Apple");
    tree.append_child(newestElement, "1");
    tree.append_child(newestElement, "2");
    tree.append_child(newestElement, "3");

    newestElement = tree.insert_after(newestElement, "Banana");
    tree.append_child(newestElement, "1");
    tree.append_child(newestElement, "2");
    tree.append_child(newestElement, "3");
    newestElement = tree.insert_after(newestElement, "Clementine");
    tree.append_child(newestElement, "1");
    tree.append_child(newestElement, "Banana");
    tree.append_child(newestElement, "3");
    newestElement = tree.insert_after(newestElement, "Druer");
    tree.append_child(newestElement, "1");
    tree.append_child(newestElement, "2");
    tree.append_child(newestElement, "3");

    // auto search = std::find(tree.begin_fixed(tree.begin(), 0), tree.end_fixed(tree.end(), 0), "Clementine");
    // ccout << *search << " " << tree.index(search) << endl;

    //    cout << "Apple: " << *tree.begin() << "\n";
    //    cout << "Apple: " << *tree.begin_fixed(tree.begin(), 0) << "\n";

    // for (auto it = tree.begin_fixed(tree.begin(), 0); it != tree.end_fixed(tree.begin(), 0); ++it)
    for (auto it = tree.begin_sibling(); it != tree.end_sibling(); ++it)
        cout << "kek: " << *it << endl;

    leaves<string> x;
    for (const auto& lol : leaves(tree))
        cout << lol << endl;

    exit(0);
}

int main(int argc, char* argv[])
{
    treetest();

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
