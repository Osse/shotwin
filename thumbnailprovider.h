#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QQuickImageProvider>

class QAbstractItemModel;
class PhotoModel;

class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider(QAbstractItemModel* photoListModel);
    ~ThumbnailProvider();

    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

    void setThumbnailDirPath(const QString& value);

private:
    QImage generateFallback(QSize size);

private:
    QString cachePath;

    PhotoModel* photoListModel;
};

#endif  // THUMBNAILPROVIDER_H
