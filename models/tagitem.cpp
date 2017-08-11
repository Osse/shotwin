#include "tagitem.h"

TagItem::TagItem(EventTreeItem* parent, const QString& tag, const QString& photoList) : EventTreeItem(parent), tag(tag), photos(photoList.split(","))
{
}

TagItem::~TagItem()
{
}

QString TagItem::displayString()
{
    return tag;
}

QPixmap TagItem::getIcon()
{
    return QPixmap();
}
