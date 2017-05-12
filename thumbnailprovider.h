#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QQuickImageProvider>

class ThumbnailProvider : public QQuickImageProvider
{
public:
    ThumbnailProvider();
    ~ThumbnailProvider();

    virtual QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize);
};

#endif  // THUMBNAILPROVIDER_H
