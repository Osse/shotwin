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
        cacheDir.mkpath(cachePath + "/thumbs360");
        cacheDir.mkpath(cachePath + "/thumbs128");
    }
}

ThumbnailProvider::~ThumbnailProvider()
{
}

QImage ThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    if (requestedSize == QSize(-1, -1))
        return QImage();

    int thumbSize = std::max(requestedSize.width(), requestedSize.height()) > 128 ? 360 : 128;

    QString thumbId = id.contains("/") ? id.section("/", 0, 0) : id;
    QString thumbPath = QString("%1/thumbs%2/%3.jpg").arg(cachePath, QString::number(thumbSize), thumbId);

    if (QFile::exists(thumbPath))
        return QImage(thumbPath);

    QString filename;
    QString type;

    if (id.contains("/")) {
        type = id.section("/", 1, 1);
        filename = id.section("/", 2);
    }
    else {
        auto matches = photoListModel->match(photoListModel->index(0, 0), PhotoModel::ThumnailRole, id);
        QModelIndex match;
        if (matches.size())
            match = matches.first();
        else
            return generateFallback(requestedSize);

        filename = photoListModel->data(match, PhotoModel::MappedFilenameRole).toString();
        type = photoListModel->data(match, PhotoModel::TypeRole).toString();
    }

    QImage image;
    if (type == "photo")
        image = generateThumbnail(thumbSize, thumbPath, filename);
    else
        image = generateVideoThumbnail(thumbSize, thumbPath, filename);

    if (image.isNull())
        image = generateFallback(requestedSize);

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

QImage ThumbnailProvider::generateThumbnail(int thumbSize, const QString& thumbPath, const QString& filename)
{
    QImageReader reader(filename);
    reader.setAutoTransform(true);

    if (!reader.canRead())
        return QImage();

    auto image = reader.read();

    auto smallRough = image.scaled(thumbSize * 2, thumbSize * 2, Qt::KeepAspectRatio, Qt::FastTransformation);
    auto thumbnail = smallRough.scaled(thumbSize, thumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    thumbnail.save(thumbPath);

    return thumbnail;
}

QImage ThumbnailProvider::generateVideoThumbnail(int thumbSize, const QString& thumbPath, const QString& filename)
{
    return QImage();
}
