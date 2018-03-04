#ifndef EVENTFILTEREDPHOTOMODEL_H
#define EVENTFILTEREDPHOTOMODEL_H

#include <QSortFilterProxyModel>

#include <vector>

class EventOrTagFilteredPhotoModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit EventOrTagFilteredPhotoModel(QObject* parent = nullptr);
    ~EventOrTagFilteredPhotoModel() = default;

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    std::vector<int> getEventIds() const;
    void setEventIds(const std::vector<int>& value);
    void setEventId(int eventId);

    std::vector<int> getPhotoIds() const;
    void setPhotoIds(const std::vector<int>& value);

signals:
    void photoFilterChanged();

private:
    void sortAndInvalidate();
    bool contains(const std::vector<int>& v, int i) const;

private:
    std::vector<int> eventIds;
    std::vector<int> photoIds;
};

#endif  // EVENTFILTEREDPHOTOMODEL_H
