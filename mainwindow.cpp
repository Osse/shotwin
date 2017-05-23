#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "eventtreemodel.h"
#include "git_version.h"
#include "hidephotosproxymodel.h"
#include "photoeventlistproxymodel.h"
#include "pictureprovider.h"
#include "settingsdialog.h"
#include "thumbnailprovider.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionOpen_database, &QAction::triggered, this, &MainWindow::openDatabase);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionAbout_Shotwin, &QAction::triggered, this, &MainWindow::aboutShotwin);
    connect(ui->actionAbout_Qt, &QAction::triggered, this, &MainWindow::aboutQt);
    connect(ui->actionSettings, &QAction::triggered, this, &MainWindow::showSettings);

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

    auto argv = QApplication::arguments();
    if (argv.length() > 1)
        openDataBaseConnection(argv[1]);
    else {
        QSettings settings;
        if (settings.contains("databasepath"))
            openDataBaseConnection(settings.value("databasepath").toString());
    }
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
    if (db.open()) {
        QSettings().setValue("databasepath", dbName);
        initModelsAndViews();
    }
    else
        QMessageBox::warning(this, tr("Failed to open database"), tr("Failed to open database."));
}

void MainWindow::initModelsAndViews()
{
    auto eventTreeModel = new EventTreeModel(this);
    auto proxy = new HidePhotosProxyModel(this);
    proxy->setSourceModel(eventTreeModel);
    ui->eventTree->setModel(proxy);
    ui->eventTree->expandAll();

    auto photoEventListProxyModel = new PhotoEventListProxyModel(this);
    photoEventListProxyModel->setSourceModel(eventTreeModel);
    connect(ui->eventTree,
            &QTreeView::clicked,
            photoEventListProxyModel,
            &PhotoEventListProxyModel::setTopLevelItemFromIndex);
    ui->photoView->rootContext()->setContextProperty("photoListModel", photoEventListProxyModel);
    ui->photoView->rootContext()->setContextProperty("shade", QSettings().value("shade", 128).toInt());
    ui->photoView->setSource(QUrl::fromLocalFile(CMAKE_SOURCE_DIR "/PhotoView.qml"));
    ui->photoView->engine()->addImageProvider("thumbnails", new ThumbnailProvider());
    ui->photoView->engine()->addImageProvider("pictures", new PictureProvider());
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

void MainWindow::showSettings()
{
    SettingsDialog sd(this);

    connect(&sd, &SettingsDialog::settingsChanged, [this]() {
        static_cast<ThumbnailProvider*>(ui->photoView->engine()->imageProvider("thumbnails"))
            ->setThumbnailDirPath(QSettings().value("cachepath").toString());
    });
    connect(&sd, &SettingsDialog::settingsChanged, [this]() {
        static_cast<PictureProvider*>(ui->photoView->engine()->imageProvider("pictures"))
            ->setMap(QSettings().value("map").toMap());
    });
    connect(&sd, &SettingsDialog::bgColorChanged, [this](int value) {
        ui->photoView->rootContext()->setContextProperty("shade", value);
    });

    sd.exec();
}
