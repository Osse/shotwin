#ifndef EVENTFILTEREDPHOTOMODEL_H
#define EVENTFILTEREDPHOTOMODEL_H

#include <QSortFilterProxyModel>

class EventFilteredPhotoModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventFilteredPhotoModel(QObject* parent = nullptr);
    ~EventFilteredPhotoModel();

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const;

    int getEventId() const;
    void setEventId(int value);

signals:
    void eventChanged();

private:
    int eventId;
};

#endif  // EVENTFILTEREDPHOTOMODEL_H
