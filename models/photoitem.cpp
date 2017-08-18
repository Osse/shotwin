#include "photoitem.h"

#include "eventtreeitem.h"

PhotoItem::PhotoItem(EventTreeItem* parent, int photoId, const QDateTime& exposureTime, const QString& fileName)
    : EventTreeItem(parent), photoId(photoId), exposureTime(exposureTime), fileName(fileName)
{
}

PhotoItem::PhotoItem(
    int photoId, int eventId, const QDateTime& exposureTime, const QString& fileName, const QString& type)
    : EventTreeItem(nullptr),
      photoId(photoId),
      eventId(eventId),
      exposureTime(exposureTime),
      fileName(fileName),
      type(type)
{
}

PhotoItem::~PhotoItem()
{
}

QString PhotoItem::displayString() const
{
    return fileName;
}

QString PhotoItem::getThumbnailId() const
{
    if (type == "photo")
        return QString("thumb%1").arg(photoId, 16, 16, QChar('0'));
    else
        return QString("video-%1").arg(photoId, 16, 16, QChar('0'));
}

QString PhotoItem::getFilename() const
{
    return fileName;
}

QPixmap PhotoItem::getIcon() const
{
    return QPixmap();
}

int PhotoItem::sortData() const
{
    return exposureTime.toSecsSinceEpoch();
}

QDateTime PhotoItem::getExposureTime() const
{
    return exposureTime;
}

int PhotoItem::getEventId() const
{
    return eventId;
}

QString PhotoItem::getType() const
{
    return type;
}

int PhotoItem::getPhotoId() const
{
    return photoId;
}

bool operator<(const PhotoItem& lhs, const PhotoItem& rhs)
{
    return lhs.exposureTime < rhs.exposureTime;
}
