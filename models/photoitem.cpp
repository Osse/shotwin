#include "photoitem.h"

PhotoItem::PhotoItem(
    int photoId, int eventId, const QDateTime& exposureTime, const QString& fileName, const QString& type, int rating)
    : photoId(photoId), eventId(eventId), exposureTime(exposureTime), fileName(fileName), type(type), rating(rating)
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

int PhotoItem::getRating() const
{
    return rating;
}

int PhotoItem::getPhotoId() const
{
    return photoId;
}

bool operator<(const PhotoItem& lhs, const PhotoItem& rhs)
{
    return lhs.getExposureTime() < rhs.getExposureTime();
}
