#ifndef EVENTMODEL_H
#define EVENTMODEL_H

#include <QAbstractListModel>

#include "eventitem.h"

class EventModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit EventModel(QObject* parent = nullptr);
    ~EventModel() = default;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    enum UserRoles {
        ThumnailRole = Qt::UserRole + 1,
        StartTimeRole,
        EndTimeRole,
        FilenameRole,
        TimespanRole,
        PhotoCountRole,
        VideoCountRole
    };

private:
    void init();

private:
    QPixmap icon{":/icons/one-event.png"};
    std::vector<EventItem> eventList;
};

#endif  // EVENTMODEL_H
