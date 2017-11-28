#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "eventmodel.h"
#include "git_version.h"
#include "settingsdialog.h"
#include "shotwin.h"
#include "thumbnailprovider.h"

#include <QFileDialog>
#include <QIdentityProxyModel>
#include <QMessageBox>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSettings>
#include <QShortcut>
#include <QSqlDatabase>
#include <QStandardPaths>

class DateFormatProxyModel : public QIdentityProxyModel
{
public:
    DateFormatProxyModel(QObject* parent) : QIdentityProxyModel(parent)
    {
    }

    QVariant data(const QModelIndex& index, int role) const
    {
        if (role != Qt::DisplayRole)
            return QIdentityProxyModel::data(index, role);

        auto display = sourceModel()->data(index, role).toString();
        if (display.isEmpty())
            return sourceModel()->data(index, EventModel::StartTimeRole).toDate().toString();
        else
            return display;
    }
};

MainWindow::MainWindow(Shotwin* shotwin, QWidget* parent)
    : shotwin(shotwin), QMainWindow(parent), ui(new Ui::MainWindow)
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
    if (!shotwin->initModels())
        QMessageBox::warning(this, tr("Error"), tr("Failed to set up models."));

    setupTree(ui->eventTree);
    auto proxy = new DateFormatProxyModel(this);
    proxy->setSourceModel(shotwin->getEventTree());
    ui->eventTree->setModel(proxy);
    ui->eventTree->expandAll();
    connect(ui->eventTree, &QTreeView::clicked, shotwin, &Shotwin::selectEvent);
    connect(shotwin, &Shotwin::eventSelected, ui->eventTree, &QTreeView::setCurrentIndex);

    setupTree(ui->tags);
    ui->tags->setModel(shotwin->getTagModel());
    connect(ui->tags, &QTreeView::clicked, shotwin, &Shotwin::selectTag);
    connect(shotwin, &Shotwin::tagSelected, ui->tags, &QTreeView::setCurrentIndex);

    setupTree(ui->fileTree);
    ui->fileTree->setModel(shotwin->getFileSystemModel());
    int depth = 0;
    QModelIndex index;
    while (ui->fileTree->model()->rowCount(index) == 1) {
        index = ui->fileTree->model()->index(0, 0, index);
        depth++;
    }
    ui->fileTree->expandToDepth(depth - 1);

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
    if (eventView)
        connect(eventView, SIGNAL(eventDoubleClicked(int)), shotwin, SLOT(openEvent(int)));

    QObject* photoView = ui->photoView->rootObject()->findChild<QObject*>("photoView");
    if (photoView)
        connect(photoView, SIGNAL(tagClicked(QString)), shotwin, SLOT(selectTagByString(QString)));
}

void MainWindow::setupTree(QTreeView* tree)
{
    tree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tree->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    tree->setSizePolicy(tree->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    tree->header()->setSectionsClickable(true);
    connect(tree->header(), &QHeaderView::sectionClicked, shotwin, &Shotwin::resetFilterAndShowTree);
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
        provider->setThumbnailDirPath(QSettings().value("cachepath").toString());
        provider->setFfmpegCmd(QSettings().value("ffmpegcmd").toString());
    });
    connect(&sd, &SettingsDialog::settingsChanged, [this]() { shotwin->setMap(QSettings().value("map").toMap()); });
    connect(&sd, &SettingsDialog::bgColorChanged, [this](int value) {
        ui->photoView->rootContext()->setContextProperty("shade", value);
    });

    sd.exec();
}
