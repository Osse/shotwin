#include "eventitem.h"

#include "eventtreeitem.h"
#include "photoitem.h"

EventItem::EventItem(EventTreeItem* parent, int eventId, const QString& eventName, const QDateTime& startTime)
    : EventTreeItem(parent), eventId(eventId), startTime(startTime), eventName(eventName)
{
}

EventItem::~EventItem()
{
}

QString EventItem::displayString()
{
    if (!eventName.isEmpty())
        return eventName;
    else
        return createEventName();
}

QString EventItem::createEventName()
{
    auto firstPhoto = static_cast<PhotoItem*>(children.front());
    return firstPhoto->getExposureTime().date().toString();
}
