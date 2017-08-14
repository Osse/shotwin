#ifndef ROOTITEM_H
#define ROOTITEM_H

#include "eventtreeitem.h"

class RootItem : public EventTreeItem
{
public:
    RootItem();
    ~RootItem();

    QPixmap getIcon() const override;
};

#endif  // ROOTITEM_H
