#include "yearitem.h"

YearItem::YearItem(EventTreeItem* parent, int year) : EventTreeItem(parent), year(year)
{
}

YearItem::~YearItem()
{
}

QString YearItem::displayString()
{
    return QString::number(year);
}
