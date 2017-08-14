#include "photoitem.h"

#include "eventtreeitem.h"

#include <exiv2/jpgimage.hpp>

#include <QDebug>

PhotoItem::PhotoItem(EventTreeItem* parent,
                     int photoId,
                     int eventId,
                     const QDateTime& exposureTime,
                     const QString& fileName,
                     const QString& mappedFileName)
    : EventTreeItem(parent),
      photoId(photoId),
      eventId(eventId),
      exposureTime(exposureTime),
      fileName(fileName),
      mappedFileName(mappedFileName)
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
    return QString("thumb%1").arg(photoId, 16, 16, QChar('0'));
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

bool operator<(const PhotoItem& lhs, const PhotoItem& rhs)
{
    return lhs.exposureTime < rhs.exposureTime;
}

void PhotoItem::populateFromExif()
{
    if (populatedDone)
        return;

    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(mappedFileName.toStdString());
    image->readMetadata();
    auto& exifData = image->exifData();

    size.rwidth() = exifData["Exif.Photo.PixelXDimension"].value().toLong();
    size.rheight() = exifData["Exif.Photo.PixelYDimension"].value().toLong();

    auto speed = exifData["Exif.Photo.ExposureTime"].toRational();
    auto fNumber = exifData["Exif.Photo.FNumber"].toRational();
    auto iso = exifData["Exif.Photo.ISOSpeedRatings"].toLong();

    exposureString =
        exposureString.arg(speed.first).arg(speed.second).arg(1.0 * fNumber.first / fNumber.second).arg(iso);

    populatedDone = true;
}

int PhotoItem::getEventId() const
{
    return eventId;
}

QString PhotoItem::getExposureString() const
{
    return exposureString;
}

QSize PhotoItem::getSize() const
{
    return size;
}
