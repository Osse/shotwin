#ifndef EVENTFILTEREDPHOTOMODEL_H
#define EVENTFILTEREDPHOTOMODEL_H

#include <QSortFilterProxyModel>

class EventOrTagFilteredPhotoModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventOrTagFilteredPhotoModel(QObject* parent = nullptr);
    ~EventOrTagFilteredPhotoModel();

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    QList<int> getEventIds() const;
    void setEventIds(const QList<int>& value);
    void setEventId(int eventId);

    QList<int> getPhotoIds() const;
    void setPhotoIds(const QList<int>& value);

signals:
    void filterChanged();

private:
    void sortAndInvalidate();

private:
    QList<int> eventIds;
    QList<int> photoIds;
};

#endif  // EVENTFILTEREDPHOTOMODEL_H
