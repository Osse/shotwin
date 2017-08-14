#ifndef EVENTITEM_H
#define EVENTITEM_H

#include "eventtreeitem.h"

#include <QDateTime>

class EventItem : public EventTreeItem
{
public:
    EventItem(EventTreeItem* parent, int eventId, const QString& eventName, const QString& primarySourceId);
    ~EventItem();

    QString displayString() const override;
    QString getThumbnailId() const override;
    QString getEventTimeSpan() const override;
    QPixmap getIcon() const override;
    QDateTime getStartTime() const;
    QDateTime getEndTime() const;
    int sortData() const override;
    int getEventId() const;

    friend bool operator<(const EventItem& lhs, const EventItem& rhs);

private:
    QString createEventName() const;

private:
    int eventId;
    QString eventName;
    QString primarySourceId;
};

#endif  // EVENTITEM_H
