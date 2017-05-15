#include "eventitem.h"

#include "eventtreeitem.h"
#include "photoitem.h"

#include <QApplication>
#include <QStyle>

EventItem::EventItem(EventTreeItem* parent,
                     int eventId,
                     const QString& eventName,
                     const QDateTime& startTime,
                     const QString& primarySourceId)
    : EventTreeItem(parent),
      eventId(eventId),
      startTime(startTime),
      eventName(eventName),
      primarySourceId(primarySourceId)
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

QString EventItem::getThumbnailId()
{
    return primarySourceId;
}

QPixmap EventItem::getIcon()
{
    auto style = QApplication::style();
    return style->standardPixmap(QStyle::SP_FileIcon);
}

QString EventItem::createEventName()
{
    auto firstPhoto = static_cast<PhotoItem*>(children.front());
    return firstPhoto->getExposureTime().date().toString();
}

bool operator<(const EventItem& lhs, const EventItem& rhs)
{
    return lhs.startTime < rhs.startTime;
}
