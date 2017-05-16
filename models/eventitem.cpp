#include "eventitem.h"

#include "eventtreeitem.h"
#include "photoitem.h"

#include <QApplication>
#include <QStyle>

EventItem::EventItem(EventTreeItem* parent, int eventId, const QString& eventName, const QString& primarySourceId)
    : EventTreeItem(parent), eventId(eventId), eventName(eventName), primarySourceId(primarySourceId)
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

QDateTime EventItem::getStartTime() const
{
    if (children.size()) {
        auto photo = static_cast<PhotoItem*>(children.front());
        return photo->getExposureTime();
    }

    return QDateTime();
}

QDateTime EventItem::getEndTime() const
{
    if (children.size()) {
        auto photo = static_cast<PhotoItem*>(children.back());
        return photo->getExposureTime();
    }

    return QDateTime();
}

QString EventItem::createEventName()
{
    return getStartTime().date().toString();
}

bool operator<(const EventItem& lhs, const EventItem& rhs)
{
    return lhs.getStartTime() < rhs.getStartTime();
}

int EventItem::sortData()
{
    return -getStartTime().toSecsSinceEpoch();
}
