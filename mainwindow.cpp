#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "git_version.h"
#include "pictureprovider.h"
#include "settingsdialog.h"
#include "shotwin.h"
#include "thumbnailprovider.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QStandardPaths>
#include <QTableView>

MainWindow::MainWindow(Shotwin* shotwin, QWidget* parent)
    : shotwin(shotwin), QMainWindow(parent), ui(new Ui::MainWindow)
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
        ui->photoView->rootContext()->setContextProperty("stackIndex", 1);
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
        initDbViews();
        initModelsAndViews();
    }
    else
        QMessageBox::warning(this, tr("Failed to open database"), tr("Failed to open database."));
}

void MainWindow::initModelsAndViews()
{
    shotwin->initModels();

    ui->eventTree->setModel(shotwin->getEventTree());
    ui->eventTree->expandAll();
    connect(ui->eventTree, &QTreeView::clicked, shotwin, &Shotwin::selectEvent);
    connect(shotwin, &Shotwin::eventSelected, ui->eventTree, &QTreeView::setCurrentIndex);
    connect(shotwin, &Shotwin::photoInfoReady, ui->infoBox, &InfoBox::showPhotoInfo);
    connect(shotwin, &Shotwin::eventInfoReady, ui->infoBox, &InfoBox::showEventInfo);

    ui->photoView->engine()->addImageProvider("thumbnails", new ThumbnailProvider());
    ui->photoView->engine()->addImageProvider("pictures", new PictureProvider());

    auto rootContext = ui->photoView->rootContext();
    qmlRegisterType<Shotwin>("shotwin", 1, 0, "shotwin");
    rootContext->setContextProperty("shotwin", shotwin);

    rootContext->setContextProperty("photoListModel", shotwin->getPhotoList());
    rootContext->setContextProperty("eventListModel", shotwin->getEventList());
    rootContext->setContextProperty("shade", QSettings().value("shade", 128).toInt());
    ui->photoView->setSource(QUrl::fromLocalFile(CMAKE_SOURCE_DIR "/Main.qml"));

    QObject* eventView = ui->photoView->rootObject()->findChild<QObject*>("eventView");
    if (eventView) {
        connect(eventView, SIGNAL(eventDoubleClicked(int)), shotwin, SLOT(openEvent(int)));
        connect(eventView, SIGNAL(eventSelected(int)), shotwin, SLOT(handleEventSelected(int)));
    }

    QObject* photoView2 = ui->photoView->rootObject()->findChild<QObject*>("photoView");
    if (photoView2)
        connect(photoView2, SIGNAL(photoSelected(int)), shotwin, SLOT(handlePhotoSelected(int)));
}

void MainWindow::initDbViews()
{
    QString createPhotoVideoView(
        "create temporary view PhotoVideoView as "
        "select id, event_id, exposure_time, filename, 'photo' as type from PhotoTable "
        "union all "
        "select id, event_id, exposure_time, filename, 'video' as type from VideoTable;");

    QString createEventViewWithStartTime(
        "create temporary view EventViewWithTimes as "
        "select e.id, e.name, pv.start_time, pv.end_time, e.primary_source_id from ( "
        "select event_id, min(exposure_time) as start_time, max(exposure_time) as end_time from PhotoVideoView group "
        "by event_id) pv "
        "left outer join EventTable as e on pv.event_id = e.id;");

    QSqlQuery query(QSqlDatabase::database());

    if (!query.exec(createPhotoVideoView))
        QMessageBox::warning(this, tr("Database error"), tr("Failed to create needed photo view"));

    if (!query.exec(createEventViewWithStartTime))
        QMessageBox::warning(this, tr("Database error"), tr("Failed to create needed event view"));

    auto m = new QSqlTableModel(this, QSqlDatabase::database());
    m->setTable("EventViewWithTimes");
    m->select();

    auto w = new QTableView();
    w->setModel(m);

    auto l = new QVBoxLayout();
    l->addWidget(w);

    auto d = new QDialog(this);
    connect(qApp, &QApplication::aboutToQuit, d, &QDialog::close);
    d->setLayout(l);
    d->setGeometry(100, 100, 800, 500);
    d->show();
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
