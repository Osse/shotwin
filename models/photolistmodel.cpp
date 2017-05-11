#include "photolistmodel.h"

#include "eventitem.h"

PhotoListModel::PhotoListModel(QObject* parent) : QAbstractListModel(parent)
{
}

PhotoListModel::~PhotoListModel()
{
}

int PhotoListModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid() && event)
        return event->childCount();
    else
        return 0;
}

QVariant PhotoListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    if (!event)
        return QVariant();

    auto photo = event->getChild(index.row());
    return photo->displayString();
}

EventItem* PhotoListModel::getEvent() const
{
    return event;
}

void PhotoListModel::setEventFromIndex(const QModelIndex& index)
{
    setEvent(static_cast<EventItem*>(index.internalPointer()));
}

void PhotoListModel::setEvent(EventItem* newEvent)
{
    beginResetModel();
    event = newEvent;
    endResetModel();
}
