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

QString EventItem::displayString() const
{
    if (!eventName.isEmpty())
        return eventName;
    else
        return createEventName();
}

QString EventItem::getThumbnailId() const
{
    return primarySourceId;
}

QString EventItem::getEventTimeSpan() const
{
    auto start = getStartTime().date();
    auto end = getEndTime().date();

    QString startFormat;
    QString endFormat;
    QString span{"%1 - %2"};
    bool twoDates = true;

    if (start.year() != end.year()) {
        startFormat = "MMM dd, yyyy";
        endFormat = startFormat;
    }
    else if (start.month() != end.month()) {
        startFormat = "MMM dd";
        endFormat = startFormat;
    }
    else if (start.day() != end.day()) {
        startFormat = "MMM dd";
        endFormat = "dd";
    }
    else {
        startFormat = "MMM dd";
        span = "%1";
        twoDates = false;
    }

    if (twoDates)
        return QString(span).arg(getStartTime().date().toString(startFormat), getEndTime().date().toString(endFormat));
    else
        return QString(span).arg(getStartTime().date().toString(startFormat));
}

QPixmap EventItem::getIcon() const
{
    auto style = QApplication::style();
    return style->standardPixmap(QStyle::SP_FileIcon);
}

QDateTime EventItem::getStartTime() const
{
    if (children.size()) {
        auto photo = std::dynamic_pointer_cast<PhotoItem>(children.front());
        return photo->getExposureTime();
    }

    return QDateTime();
}

QDateTime EventItem::getEndTime() const
{
    if (children.size()) {
        auto photo = std::dynamic_pointer_cast<PhotoItem>(children.back());
        return photo->getExposureTime();
    }

    return QDateTime();
}

QString EventItem::createEventName() const
{
    return getStartTime().date().toString();
}

bool operator<(const EventItem& lhs, const EventItem& rhs)
{
    return lhs.getStartTime() < rhs.getStartTime();
}

int EventItem::sortData() const
{
    return -getStartTime().toSecsSinceEpoch();
}
