#ifndef EVENTITEM_H
#define EVENTITEM_H

#include "eventtreeitem.h"

#include <QDateTime>

class EventItem : public EventTreeItem
{
public:
    EventItem(EventTreeItem* parent, int eventId, const QString& eventName, const QDateTime& startTime);
    ~EventItem();

    QString displayString() override;

private:
    QString createEventName();

private:
    int eventId;
    QString eventName;
};

#endif  // EVENTITEM_H
