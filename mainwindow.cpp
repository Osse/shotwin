#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "eventtreemodel.h"
#include "git_version.h"
#include "photolistmodel.h"
#include "thumbnailprovider.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QStandardPaths>
#include <QQmlContext>
#include <QShortcut>
#include <QQmlEngine>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen_database, &QAction::triggered, this, &MainWindow::openDatabase);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionAbout_Shotwin, &QAction::triggered, this, &MainWindow::aboutShotwin);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQt);

    auto reloadQml = new QShortcut(Qt::Key_F5, this);
    connect(reloadQml, &QShortcut::activated, [this]() {
        auto pv = ui->photoView;
        auto tmp = pv->source();
        pv->setSource(QUrl());
        pv->engine()->clearComponentCache();
        pv->setSource(tmp);
    });

    ui->splitter->setSizes({33000, 67000});
    ui->mainToolBar->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDatabase()
{
    auto home = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    auto dbName = QFileDialog::getOpenFileName(this, "Choose Shotwell database", home);

    if (!dbName.isEmpty())
        openDataBaseConnection(dbName);
}

void MainWindow::openDataBaseConnection(const QString& dbName)
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);
    if (db.open())
        initModelsAndViews();
    else
        QMessageBox::warning(this, tr("Failed to open database"), tr("Failed to open database."));
}

void MainWindow::initModelsAndViews()
{
    auto eventTreeModel = new EventTreeModel(this);
    ui->eventTree->setModel(eventTreeModel);
    ui->eventTree->expandAll();

    auto photoListModel = new PhotoListModel(this);
    connect(ui->eventTree, &QTreeView::clicked, photoListModel, &PhotoListModel::setEventFromIndex);
    ui->photoView->setSource(QUrl::fromLocalFile(CMAKE_SOURCE_DIR "/PhotoView.qml"));
    ui->photoView->rootContext()->setContextProperty("photoListModel", photoListModel);
    ui->photoView->engine()->addImageProvider("thumbnails", new ThumbnailProvider());
}

void MainWindow::aboutShotwin()
{
    auto aboutText = tr("Simple Qt-based view onto a Shotwell database. Version %1.");
    QMessageBox::about(this, tr("About QtShot"), aboutText.arg(GIT_VERSION));
}

void MainWindow::aboutQt()
{
    QMessageBox::aboutQt(this);
}
