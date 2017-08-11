#ifndef HEADERITEM_H
#define HEADERITEM_H

#include "eventtreeitem.h"

class HeaderItem : public EventTreeItem
{
public:
    HeaderItem(EventTreeItem* parent, const QString& title);
    ~HeaderItem();

    QString displayString() override;
    QPixmap getIcon() override;

private:
    QString title;
};

#endif // HEADERITEM_H
