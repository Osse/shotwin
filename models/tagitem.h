#ifndef TAGITEM_H
#define TAGITEM_H

#include "eventtreeitem.h"

class TagItem : public EventTreeItem
{
public:
    TagItem(EventTreeItem* parent, const QString& tag, const QString& photoList);
    ~TagItem();

    QString displayString() override;
    QPixmap getIcon() override;

private:
    QString tag;
    QStringList photos;
};

#endif // TAGITEM_H
