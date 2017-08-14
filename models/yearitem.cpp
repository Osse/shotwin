#include "yearitem.h"

#include "monthitem.h"

#include <QApplication>
#include <QStyle>

YearItem::YearItem(EventTreeItem* parent, int year) : EventTreeItem(parent), year(year)
{
}

YearItem::~YearItem()
{
}

QString YearItem::displayString() const
{
    return QString::number(year);
}

QPixmap YearItem::getIcon() const
{
    auto style = QApplication::style();
    return style->standardPixmap(QStyle::SP_DirClosedIcon);
}

int YearItem::sortData() const
{
    return -year;
}

bool operator<(const YearItem& lhs, const YearItem& rhs)
{
    return lhs.year < rhs.year;
}
