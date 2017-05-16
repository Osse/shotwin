#ifndef EVENTTREEITEM_H
#define EVENTTREEITEM_H

#include <QPixmap>
#include <QString>

#include <vector>

class EventTreeItem
{
public:
    EventTreeItem(EventTreeItem* parent);
    virtual ~EventTreeItem() = 0;

    int childCount();
    virtual QString displayString();
    virtual QString getThumbnailId();
    virtual QPixmap getIcon() = 0;

    void sortChildren();
    virtual int sortData();
    void appendChild(EventTreeItem* child);
    EventTreeItem* getChild(int row);
    int getRow(EventTreeItem* child);

    EventTreeItem* getParent() const;

protected:
    EventTreeItem* parent;
    std::vector<EventTreeItem*> children;
};

#endif  // EVENTTREEITEM_H
