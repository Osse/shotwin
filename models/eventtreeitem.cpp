#include "eventtreeitem.h"

#include <algorithm>

EventTreeItem::EventTreeItem(EventTreeItem* parent) : parent(parent)
{
}

EventTreeItem::~EventTreeItem()
{
}

int EventTreeItem::childCount()
{
    return children.size();
}

QString EventTreeItem::displayString()
{
    return QString();
}

QString EventTreeItem::getThumbnailId()
{
    return QString();
}

void EventTreeItem::sortChildren()
{
    std::for_each(children.begin(), children.end(), [](auto c) { c->sortChildren(); });
    std::sort(children.begin(), children.end(), [](auto lhs, auto rhs) { return lhs->sortData() < rhs->sortData(); });
}

int EventTreeItem::sortData()
{
    return 0;
}

void EventTreeItem::appendChild(EventTreeItem* child)
{
    children.push_back(child);
}

EventTreeItem* EventTreeItem::getChild(int row)
{
    if (row < 0 || row > children.size())
        return nullptr;
    return children.at(row);
}

EventTreeItem* EventTreeItem::getParent() const
{
    return parent;
}

int EventTreeItem::getRow(EventTreeItem* child)
{
    auto childIt = std::find(children.begin(), children.end(), child);
    if (childIt != children.end())
        return childIt - children.begin();
    else
        return -1;
}
