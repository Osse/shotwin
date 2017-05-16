#ifndef YEARITEM_H
#define YEARITEM_H

#include "eventtreeitem.h"

class YearItem : public EventTreeItem
{
public:
    YearItem(EventTreeItem* parent, int year);
    ~YearItem();

    QString displayString() override;
    QPixmap getIcon() override;
    int sortData() override;

    friend bool operator<(const YearItem& lhs, const YearItem& rhs);

private:
    int year;
};

#endif  // YEARITEM_H
