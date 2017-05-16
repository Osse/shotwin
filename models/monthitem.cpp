#include "monthitem.h"

#include "eventitem.h"

#include <QApplication>
#include <QDate>
#include <QStyle>

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

QPixmap MonthItem::getIcon()
{
    auto style = QApplication::style();
    return style->standardPixmap(QStyle::SP_DirClosedIcon);
}

int MonthItem::sortData()
{
    return -month;
}

bool operator<(const MonthItem& lhs, const MonthItem& rhs)
{
    return lhs.month < rhs.month;
}
