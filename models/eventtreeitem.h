#ifndef EVENTTREEITEM_H
#define EVENTTREEITEM_H

#include <QPixmap>
#include <QString>

#include <memory>
#include <vector>

class EventTreeItem
{
public:
    EventTreeItem(EventTreeItem* parent);
    virtual ~EventTreeItem() = 0;

    int childCount() const;
    virtual QString displayString() const;
    virtual QString getThumbnailId() const;
    virtual QString getEventTimeSpan() const;
    virtual QString getFilename() const;
    virtual QPixmap getIcon() const = 0;

    void sortChildren();
    virtual int sortData() const;
    void appendChild(EventTreeItem* child);
    EventTreeItem* getChild(int row);
    int getRow(EventTreeItem* child);

    EventTreeItem* getParent() const;

protected:
    EventTreeItem* parent;
    std::vector<std::shared_ptr<EventTreeItem>> children;
};

#endif  // EVENTTREEITEM_H
