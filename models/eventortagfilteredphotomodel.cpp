#include "eventortagfilteredphotomodel.h"

#include "photomodel.h"

EventOrTagFilteredPhotoModel::EventOrTagFilteredPhotoModel(QObject* parent) : QSortFilterProxyModel(parent)
{
    setSortRole(PhotoModel::ExposureTimeRole);
}

EventOrTagFilteredPhotoModel::~EventOrTagFilteredPhotoModel()
{
}

bool EventOrTagFilteredPhotoModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (eventIds.size() && photoIds.size() == 0) {
        auto index = sourceModel()->index(source_row, 0, source_parent);
        int eventId = sourceModel()->data(index, PhotoModel::EventIdRole).toInt();
        return eventIds.contains(eventId);
    }
    else if (eventIds.size() == 0 && photoIds.size()) {
        auto index = sourceModel()->index(source_row, 0, source_parent);
        int eventId = sourceModel()->data(index, PhotoModel::PhotoIdRole).toInt();
        return photoIds.contains(eventId);
    }

    return true;
}

void EventOrTagFilteredPhotoModel::setEventId(int eventId)
{
    setEventIds({eventId});
}

QList<int> EventOrTagFilteredPhotoModel::getPhotoIds() const
{
    return photoIds;
}

void EventOrTagFilteredPhotoModel::setPhotoIds(const QList<int>& value)
{
    photoIds = value;
    eventIds.clear();
    sortAndInvalidate();
}

QList<int> EventOrTagFilteredPhotoModel::getEventIds() const
{
    return eventIds;
}

void EventOrTagFilteredPhotoModel::setEventIds(const QList<int>& value)
{
    eventIds = value;
    photoIds.clear();
    sortAndInvalidate();
}

void EventOrTagFilteredPhotoModel::sortAndInvalidate()
{
    sort(0, eventIds.size() != 1 ? Qt::DescendingOrder : Qt::AscendingOrder);
    invalidateFilter();
    emit filterChanged();
}
