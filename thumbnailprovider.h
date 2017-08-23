#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QQuickImageProvider>

class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider();
    ~ThumbnailProvider();

    virtual QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);

    void setThumbnailDirPath(const QString& value);

private:
    QImage generateFallback(QSize size);

private:
    QString cachePath;
};

#endif  // THUMBNAILPROVIDER_H
