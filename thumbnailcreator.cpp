#include "thumbnailcreator.h"

#include <QDebug>
#include <QDir>
#include <QImage>
#include <QImageReader>
#include <QMap>
#include <QStandardPaths>

ThumbnailCreator::ThumbnailCreator(QObject* parent) : QObject(parent)
{
    worker = new Worker();
    worker->moveToThread(&thread);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(this, &ThumbnailCreator::operate, worker, &Worker::generateThumbnails);
    connect(worker, &Worker::oneDone, this, &ThumbnailCreator::handleOneDone);
    thread.start();
    thread.setPriority(QThread::LowPriority);
}

ThumbnailCreator::~ThumbnailCreator()
{
    worker->requestStop();
    thread.quit();
    thread.wait();
}

void ThumbnailCreator::start(const QMap<QString, QString>& files)
{
    worker->setFiles(files);
    emit operate();
}

void ThumbnailCreator::handleOneDone(const QString& thumbnail)
{
    qDebug() << "thumbnail ready" << thumbnail;
}

Worker::Worker(QObject* parent) : QObject(parent)
{
    auto cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    if (cachePaths.length()) {
        cachePath = cachePaths.first();
        QDir cacheDir(cachePath);
        if (cacheDir.mkpath(cachePath + "/thumbs360"))
            qDebug() << "created" << cachePath + "/thumbs360";
        else
            qDebug() << "failed" << cachePath + "/thumbs360";

        if (cacheDir.mkpath(cachePath + "/thumbs128"))
            qDebug() << "created" << cachePath + "/thumbs128";
        else
            qDebug() << "failed" << cachePath + "/thumbs128";
    }
}

Worker::~Worker()
{
}

void Worker::generateThumbnails()
{
    if (cachePath.isEmpty())
        return;

    for (auto it = files.begin(); it != files.end(); it++) {
        if (shouldStop) {
            qDebug() << "breaking";
            break;
        }

        if (generateOneThumbnail(it.key(), it.value()))
            emit oneDone(it.key());
    }
    if (!shouldStop) {
        files.clear();
        emit allDone();
    }
}

void Worker::setFiles(const QMap<QString, QString>& value)
{
    files = value;
}

void Worker::requestStop()
{
    shouldStop = true;
}

bool Worker::generateOneThumbnail(const QString& thumbnail, const QString& filename)
{
    QString path{"%1/%2/%3.jpg"};
    auto path128 = path.arg(cachePath, "thumbs128", thumbnail);
    auto path360 = path.arg(cachePath, "thumbs360", thumbnail);

    bool exists128;
    {
        QImage img(path128);
        exists128 = !img.isNull();
    }

    bool exists360;
    {
        QImage img(path360);
        exists360 = !img.isNull();
    }

    if (exists128 && exists360)
        return true;

    QImageReader reader(filename);
    reader.setAutoTransform(true);

    if (!reader.canRead())
        return false;

    auto image = reader.read();

    if (!exists128) {
        int smallSize = 128;
        auto smallRough = image.scaled(smallSize * 2, smallSize * 2, Qt::KeepAspectRatio, Qt::FastTransformation);
        auto thumb128 = smallRough.scaled(smallSize, smallSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        exists128 = thumb128.save(path128);
    }

    if (!exists360) {
        int largeSize = 360;
        auto largeRough = image.scaled(largeSize * 2, largeSize * 2, Qt::KeepAspectRatio, Qt::FastTransformation);
        auto thumb360 = largeRough.scaled(largeSize, largeSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        exists360 = thumb360.save(path360);
    }

    return exists128 && exists360;
}
