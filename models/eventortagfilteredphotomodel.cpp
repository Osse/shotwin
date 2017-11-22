#include "eventortagfilteredphotomodel.h"

#include "photomodel.h"

#include <algorithm>

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
        return contains(eventIds, eventId);
    }
    else if (eventIds.size() == 0 && photoIds.size()) {
        auto index = sourceModel()->index(source_row, 0, source_parent);
        int eventId = sourceModel()->data(index, PhotoModel::PhotoIdRole).toInt();
        return contains(photoIds, eventId);
    }

    return true;
}

void EventOrTagFilteredPhotoModel::setEventId(int eventId)
{
    setEventIds({eventId});
}

std::vector<int> EventOrTagFilteredPhotoModel::getPhotoIds() const
{
    return photoIds;
}

void EventOrTagFilteredPhotoModel::setPhotoIds(const std::vector<int>& value)
{
    photoIds = value;
    eventIds.clear();
    sortAndInvalidate();
}

std::vector<int> EventOrTagFilteredPhotoModel::getEventIds() const
{
    return eventIds;
}

void EventOrTagFilteredPhotoModel::setEventIds(const std::vector<int>& value)
{
    eventIds = value;
    photoIds.clear();
    sortAndInvalidate();
}

void EventOrTagFilteredPhotoModel::sortAndInvalidate()
{
    // Showing a specific event should list photos in chronological
    // order, but in other cases they should be in decending other
    sort(0, eventIds.size() != 1 ? Qt::DescendingOrder : Qt::AscendingOrder);
    invalidateFilter();
    emit filterChanged();
}

bool EventOrTagFilteredPhotoModel::contains(const std::vector<int>& v, int i) const
{
    return std::find(v.begin(), v.end(), i) != v.end();
}
