#include "monthitem.h"

#include <QDate>

MonthItem::MonthItem(EventTreeItem* parent, int month) : EventTreeItem(parent), month(month)
{
}

MonthItem::~MonthItem()
{
}

QString MonthItem::displayString()
{
    return QDate::longMonthName(month);
}

bool operator<(const MonthItem& lhs, const MonthItem& rhs)
{
    return lhs.month < rhs.month;
}
