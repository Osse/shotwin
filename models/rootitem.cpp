#include "rootitem.h"

#include "yearitem.h"

RootItem::RootItem() : EventTreeItem(nullptr)
{
}

RootItem::~RootItem()
{
}

QPixmap RootItem::getIcon()
{
    return QPixmap();
}
