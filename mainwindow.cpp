#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "eventmodel.h"
#ifndef GIT_VERSION
#include "git_version.h"
#endif
#include "settingsdialog.h"
#include "shotwin.h"
#include "thumbnailprovider.h"

#include <QFileDialog>
#include <QIdentityProxyModel>
#include <QMessageBox>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWidget>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QString>
#include <QVariant>

MainWindow::MainWindow(Shotwin* shotwin, QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), shotwin(shotwin)
{
    ui->setupUi(this);

    connect(ui->actionOpen_database, &QAction::triggered, this, &MainWindow::openDatabase);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionAbout_Shotwin, &QAction::triggered, this, &MainWindow::aboutShotwin);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQt);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showSettings);

#ifdef CMAKE_SOURCE_DIR
    auto reloadQml = new QShortcut(Qt::Key_F5, this);
    connect(reloadQml, &QShortcut::activated, [this]() {
        auto pv = ui->photoView;
        auto tmp = pv->source();
        pv->setSource(QUrl());
        pv->engine()->clearComponentCache();
        pv->setSource(tmp);
        ui->photoView->rootContext()->setContextProperty("stackIndex", 1);
    });
#endif

    ui->splitter->setSizes({33000, 67000});
    ui->mainToolBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleArgs(const Args& args)
{
    QSettings settings;
    QMap<QString, QVariant> map;

    if (!args.mapList.isEmpty()) {
        if (args.readConfig)
            map = settings.value("map").toMap();

        for (const auto& string : args.mapList) {
            auto parts = string.split(";");
            map[parts[0]] = parts[1];
        }

        if (args.updateConfig)
            settings.setValue("map", map);

        shotwin->setMap(map);
    }
    else
        map = settings.value("map").toMap();

    shotwin->setMap(map);

    if (!args.database.isEmpty()) {
        openDataBaseConnection(args.database);
        if (args.updateConfig)
            settings.setValue("databasepath", args.database);
    }
    else if (settings.contains("databasepath"))
        openDataBaseConnection(settings.value("databasepath").toString());
}

void MainWindow::openDatabase()
{
    auto home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    auto dbName = QFileDialog::getOpenFileName(this, "Choose Shotwell database", home);

    if (!dbName.isEmpty()) {
        openDataBaseConnection(dbName);
        QSettings().setValue("databasepath", dbName);
    }
}

void MainWindow::openDataBaseConnection(const QString& dbName)
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (db.open()) {
        initModelsAndViews();
    }
    else
        QMessageBox::warning(this, tr("Failed to open database"), tr("Failed to open database."));
}

void MainWindow::initModelsAndViews()
{
    if (!shotwin->initModels())
        QMessageBox::warning(this, tr("Error"), tr("Failed to set up models."));

    setupTree(ui->eventTree);
    ui->eventTree->setModel(shotwin->getEventTree());
    ui->eventTree->expandAll();
    connect(ui->eventTree, &QTreeView::clicked, shotwin, &Shotwin::selectEvent);
    connect(shotwin, &Shotwin::eventSelected, ui->eventTree, &QTreeView::setCurrentIndex);

    setupTree(ui->tags);
    ui->tags->setModel(shotwin->getTagModel());
    connect(ui->tags, &QTreeView::clicked, shotwin, &Shotwin::selectTag);
    connect(shotwin, &Shotwin::tagSelected, ui->tags, &QTreeView::setCurrentIndex);

    setupTree(ui->fileTree);
    ui->fileTree->setModel(shotwin->getFileSystemModel());

    expandSingleNodes(ui->fileTree);

    ui->photoView->engine()->addImageProvider("thumbnails", new ThumbnailProvider(shotwin->getPhotoModel()));

    auto rootContext = ui->photoView->rootContext();
    qmlRegisterType<Shotwin>("shotwin", 1, 0, "shotwin");
    rootContext->setContextProperty("shotwin", shotwin);

    rootContext->setContextProperty("photoListModel", shotwin->getPhotoList());
    rootContext->setContextProperty("eventListModel", shotwin->getEventList());
    rootContext->setContextProperty("tagsModel", shotwin->getTagModel());
    rootContext->setContextProperty("shade", QSettings().value("shade", 128).toInt());
#ifdef CMAKE_SOURCE_DIR
    ui->photoView->setSource(QUrl::fromLocalFile(CMAKE_SOURCE_DIR "/Main.qml"));
#else
    ui->photoView->setSource(QUrl("qrc:/Main.qml"));
#endif

    QObject* eventView = ui->photoView->rootObject()->findChild<QObject*>("eventView");
    if (eventView != nullptr)
        connect(eventView, SIGNAL(eventDoubleClicked(int)), shotwin, SLOT(openEvent(int)));

    QObject* photoView = ui->photoView->rootObject()->findChild<QObject*>("photoView");
    if (photoView != nullptr)
        connect(photoView, SIGNAL(tagClicked(QString)), shotwin, SLOT(selectTagByString(QString)));
}

void MainWindow::setupTree(QTreeView* tree)
{
    tree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tree->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    tree->setSizePolicy(tree->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    tree->header()->setSectionsClickable(true);
    tree->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tree->header(), &QHeaderView::sectionClicked, shotwin, &Shotwin::resetFilterAndShowTree);
    connect(tree->header(), &QHeaderView::customContextMenuRequested, [this, tree](const QPoint& pos) {
        showTreeMenu(tree);
    });
}

void MainWindow::showTreeMenu(QTreeView* tree)
{
    QMenu menu;
    if (tree != ui->tags) {
        menu.addAction("Collapse all", tree, &QTreeView::collapseAll);
        menu.addAction("Expand all", tree, &QTreeView::expandAll);
    }
    if (tree == ui->fileTree)
        menu.addAction("Expand single nodes", [this]() { expandSingleNodes(ui->fileTree); });

    menu.addAction("Hide", [tree]() { tree->setFixedHeight(tree->header()->height()); });
    menu.addAction("Show", [tree]() {
        tree->setMinimumHeight(0);
        tree->setMaximumHeight(QWIDGETSIZE_MAX);
    });

    menu.exec(QCursor::pos());
}

void MainWindow::expandSingleNodes(QTreeView* tree)
{
    int depth = 0;
    QModelIndex index;
    while (tree->model()->rowCount(index) == 1) {
        index = tree->model()->index(0, 0, index);
        depth++;
    }
    tree->expandToDepth(depth - 1);
}

void MainWindow::aboutShotwin()
{
    auto aboutText = tr("Simple Qt-based view onto a Shotwell database. Version %1.");
    QMessageBox::about(this, tr("About Shotwin"), aboutText.arg(GIT_VERSION));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::showSettings()
{
    SettingsDialog sd(this);

    connect(&sd, &SettingsDialog::settingsChanged, [this]() {
        auto provider = static_cast<ThumbnailProvider*>(ui->photoView->engine()->imageProvider("thumbnails"));
        if (provider != nullptr)
            provider->setFfmpegCmd(QSettings().value("ffmpegcmd").toString());
    });
    connect(&sd, &SettingsDialog::settingsChanged, [this]() { shotwin->setMap(QSettings().value("map").toMap()); });
    connect(&sd, &SettingsDialog::bgColorChanged, [this](int value) {
        ui->photoView->rootContext()->setContextProperty("shade", value);
    });

    sd.exec();
}
