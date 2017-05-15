#ifndef EVENTTREEITEM_H
#define EVENTTREEITEM_H

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

    void sortChildren();
    void appendChild(EventTreeItem* child);
    EventTreeItem* getChild(int row);
    int getRow(EventTreeItem* child);

    EventTreeItem* getParent() const;

protected:
    EventTreeItem* parent;
    std::vector<EventTreeItem*> children;
};

#endif  // EVENTTREEITEM_H
