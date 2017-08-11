#include "headeritem.h"

#include <QApplication>
#include <QStyle>

HeaderItem::HeaderItem(EventTreeItem* parent, const QString& title) : EventTreeItem(parent), title(title)
{

}

HeaderItem::~HeaderItem()
{
}

QString HeaderItem::displayString()
{
    return title;
}

QPixmap HeaderItem::getIcon()
{
    return QPixmap();
}
