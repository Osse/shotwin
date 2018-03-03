#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QQuickImageProvider>

class QAbstractItemModel;
class PhotoModel;

class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider(QAbstractItemModel* photoListModel);
    ~ThumbnailProvider() = default;

    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

    void setThumbnailDirPath(const QString& value);
    void setFfmpegCmd(const QString& value);

private:
    QImage generateFallback(QSize size);
    QImage generateThumbnail(int thumbSize, const QString& thumbPath, const QString& filename);
    QImage grabVideoFrame(const QString& thumbPath, const QString& filename);

private:
    QString cachePath;
    QString ffmpegCmd;

    PhotoModel* photoListModel;
};

#endif  // THUMBNAILPROVIDER_H
