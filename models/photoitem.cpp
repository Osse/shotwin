#include "photoitem.h"

#include "eventtreeitem.h"

PhotoItem::PhotoItem(EventTreeItem* parent, int photoId, const QDateTime& exposureTime, const QString& fileName)
    : EventTreeItem(parent), photoId(photoId), exposureTime(exposureTime), fileName(fileName)
{
}

PhotoItem::~PhotoItem()
{
}

QString PhotoItem::displayString()
{
    return fileName;
}

QString PhotoItem::getThumbnailId()
{
    return QString("thumb%1").arg(photoId, 16, 16, QChar('0'));
}

QString PhotoItem::getFilename()
{
    return fileName;
}

QPixmap PhotoItem::getIcon()
{
    return QPixmap();
}

int PhotoItem::sortData()
{
    return exposureTime.toSecsSinceEpoch();
}

QDateTime PhotoItem::getExposureTime() const
{
    return exposureTime;
}

bool operator<(const PhotoItem& lhs, const PhotoItem& rhs)
{
    return lhs.exposureTime < rhs.exposureTime;
}
