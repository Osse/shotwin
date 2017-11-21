#ifndef EVENTITEM_H
#define EVENTITEM_H

#include <QDateTime>
#include <QPixmap>

class EventItem
{
public:
    EventItem(int eventId, const QString& eventName, const QString& primarySourceId);
    EventItem(int eventId,
              const QString& eventName,
              const QDateTime& startTime,
              const QDateTime& endTime,
              int photos,
              int videos,
              const QString& primarySourceId);
    ~EventItem();

    QString displayString() const;
    QString getThumbnailId() const;
    QString getEventTimeSpan() const;
    QPixmap getIcon() const;
    QDateTime getStartTime() const;
    QDateTime getEndTime() const;
    int sortData() const;
    int getEventId() const;
    int getPhotos() const;
    int getVideos() const;

    friend bool operator<(const EventItem& lhs, const EventItem& rhs);

private:
    QString createEventName() const;

private:
    int eventId;
    QString eventName;
    QString primarySourceId;

    QDateTime startTime;
    QDateTime endTime;

    int photos;
    int videos;
};

#endif  // EVENTITEM_H
