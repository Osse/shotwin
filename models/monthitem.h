#ifndef MONTHITEM_H
#define MONTHITEM_H

#include "eventtreeitem.h"

class MonthItem : public EventTreeItem
{
public:
    MonthItem(EventTreeItem* parent, int month);
    ~MonthItem();

    QString displayString() override;
    QPixmap getIcon() override;

    friend bool operator<(const MonthItem& lhs, const MonthItem& rhs);

private:
    int month;
};

#endif  // MONTHITEM_H
