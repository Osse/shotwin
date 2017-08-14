#include "eventfilteredphotomodel.h"

#include "photomodel.h"

EventFilteredPhotoModel::EventFilteredPhotoModel(QObject* parent) : QSortFilterProxyModel(parent), eventId(-1)
{
    setSortRole(PhotoModel::ExposureTimeRole);
}

EventFilteredPhotoModel::~EventFilteredPhotoModel()
{
}

bool EventFilteredPhotoModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (eventId < 0)
        return true;

    auto index = sourceModel()->index(source_row, 0, source_parent);
    return sourceModel()->data(index, PhotoModel::EventIdRole).toInt() == eventId;
}

int EventFilteredPhotoModel::getEventId() const
{
    return eventId;
}

void EventFilteredPhotoModel::setEventId(int value)
{
    eventId = value;
    sort(0, eventId < 0 ? Qt::DescendingOrder : Qt::AscendingOrder);
    invalidateFilter();
}
