#include "thumbnailprovider.h"

#include <QDebug>
#include <QFile>
#include <QSettings>

#include <algorithm>

ThumbnailProvider::ThumbnailProvider() : QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{
    cachePath = QSettings().value("cachepath", QString()).toString();
}

ThumbnailProvider::~ThumbnailProvider()
{
}

QPixmap ThumbnailProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    int maxSize = std::max(requestedSize.width(), requestedSize.height());
    QString sizeDir(maxSize > 128 ? "thumbs360" : "thumbs128");
    auto thumbnailPath = QString("%1/%2/%3.jpg").arg(cachePath, sizeDir, id);

    QPixmap pm;
    if (QFile::exists(thumbnailPath))
        pm = QPixmap(thumbnailPath);
    else
        pm = generateFallback(requestedSize);

    if (size)
        *size = pm.size();

    return pm;
}

void ThumbnailProvider::setThumbnailDirPath(const QString& value)
{
    cachePath = value;
}

QPixmap ThumbnailProvider::generateFallback(QSize size)
{
    QPixmap pm(size);
    pm.fill("steelblue");
    return pm;
}
