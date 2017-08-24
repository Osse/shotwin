#include "thumbnailprovider.h"

#include "photomodel.h"

#include <QDebug>
#include <QFile>
#include <QSettings>

#include <algorithm>

ThumbnailProvider::ThumbnailProvider(QAbstractItemModel* photoListModel)
    : QQuickImageProvider(QQmlImageProviderBase::Image), photoListModel(dynamic_cast<PhotoModel*>(photoListModel))
{
    cachePath = QSettings().value("cachepath", QString()).toString();
}

ThumbnailProvider::~ThumbnailProvider()
{
}

QImage ThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    int maxSize = std::max(requestedSize.width(), requestedSize.height());
    QString sizeDir(maxSize > 128 ? "thumbs360" : "thumbs128");
    auto thumbnailPath = QString("%1/%2/%3.jpg").arg(cachePath, sizeDir, id);

    QImage image;
    if (QFile::exists(thumbnailPath))
        image = QImage(thumbnailPath);
    else
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
