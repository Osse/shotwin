#ifndef YEARITEM_H
#define YEARITEM_H

#include "eventtreeitem.h"

class YearItem : public EventTreeItem
{
public:
    YearItem(EventTreeItem* parent, int year);
    ~YearItem();

    QString displayString() override;

private:
    int year;
};

#endif  // YEARITEM_H
