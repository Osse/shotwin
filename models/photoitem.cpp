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

QDateTime PhotoItem::getExposureTime() const
{
    return exposureTime;
}
