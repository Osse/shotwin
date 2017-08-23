#include "thumbnailprovider.h"

#include "photomodel.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QSettings>
#include <QStandardPaths>

#include <algorithm>

ThumbnailProvider::ThumbnailProvider(QAbstractItemModel* photoListModel)
    : QQuickImageProvider(QQmlImageProviderBase::Image), photoListModel(dynamic_cast<PhotoModel*>(photoListModel))
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

ThumbnailProvider::~ThumbnailProvider()
{
}

QImage ThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    int thumbSize = std::max(requestedSize.width(), requestedSize.height()) > 128 ? 360 : 128;
    auto thumbPath = QString("%1/thumbs%2/%3.jpg").arg(cachePath, QString::number(thumbSize), id);

    QImage image;

    if (QFile::exists(thumbPath))
        image = QImage(thumbPath);
    else {
        image = generateThumbnail(thumbSize, id);

        if (image.isNull())
            image = generateFallback(requestedSize);
    }

    if (size)
        *size = image.size();

    return image;
}

void ThumbnailProvider::setThumbnailDirPath(const QString& value)
{
    cachePath = value;
}

QImage ThumbnailProvider::generateFallback(QSize size)
{
    QImage image(size, QImage::Format_RGB32);
    image.fill("steelblue");
    return image;
}

QImage ThumbnailProvider::generateThumbnail(int thumbSize, const QString& id)
{
    auto matches = photoListModel->match(photoListModel->index(0, 0), PhotoModel::ThumnailRole, id);
    QModelIndex match;
    if (matches.size())
        match = matches.first();
    else
        return QImage();

    int size = thumbSize > 128 ? 360 : 128;

    auto filename = photoListModel->data(match, PhotoModel::MappedFilenameRole).toString();

    QImageReader reader(filename);
    reader.setAutoTransform(true);

    if (!reader.canRead())
        return QImage();

    auto image = reader.read();

    auto smallRough = image.scaled(size * 2, size * 2, Qt::KeepAspectRatio, Qt::FastTransformation);
    auto thumbnail = smallRough.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    auto thumbPath = QString("%1/thumbs%2/%3.jpg").arg(cachePath, QString::number(thumbSize), id);
    thumbnail.save(thumbPath);

    return thumbnail;
}
