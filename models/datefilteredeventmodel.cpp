#include "datefilteredeventmodel.h"

#include "eventmodel.h"

DateFilteredEventModel::DateFilteredEventModel(QObject* parent) : QSortFilterProxyModel(parent)
{
}

bool DateFilteredEventModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
    if (year != -1 && month == -1) {
        auto index = sourceModel()->index(source_row, 0, source_parent);
        auto startTime = sourceModel()->data(index, EventModel::StartTimeRole).toDateTime();
        return startTime.date().year() == year;
    }
    else if (year != -1 && month != -1) {
        auto index = sourceModel()->index(source_row, 0, source_parent);
        auto startTime = sourceModel()->data(index, EventModel::StartTimeRole).toDateTime();
        auto date = startTime.date();
        return date.year() == year && date.month() == month;
    }
    return true;
}

void DateFilteredEventModel::setFilterYear(int value)
{
    year = value;
    month = -1;
    invalidate();
}

void DateFilteredEventModel::setFilterMonth(const int& year, const int& month)
{
    this->year = year;
    this->month = month;
    invalidate();
}

void DateFilteredEventModel::invalidate()
{
    invalidateFilter();
    emit filterChanged();
}
