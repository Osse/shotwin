#include "thumbnailprovider.h"

#include <QDebug>
#include <QPainter>

ThumbnailProvider::ThumbnailProvider() : QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{
}

ThumbnailProvider::~ThumbnailProvider()
{
}

QPixmap ThumbnailProvider::requestPixmap(const QString& id, QSize* size, const QSize& requestedSize)
{
    qDebug() << "id:" << id;
    qDebug() << "requestedSize:" << requestedSize;

    QSize actualSize(64, 128);

    QPixmap pm(actualSize);
    pm.fill("steelblue");

    QPainter painter(&pm);
    painter.setPen("black");

    QString modifiableId(id);
    modifiableId.remove(0, id.lastIndexOf("/") + 1);

    painter.drawText(10, 20, modifiableId);

    if (size)
        *size = actualSize;

    return pm;
}
