#include "photoeventlistproxymodel.h"

#include "eventitem.h"
#include "eventtreeitem.h"
#include "monthitem.h"
#include "yearitem.h"

#include <QAbstractProxyModel>

PhotoEventListProxyModel::PhotoEventListProxyModel(QObject* parent) : QAbstractProxyModel(parent)
{
}

QModelIndex PhotoEventListProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!sourceModel()->hasIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.parent()))
        return QModelIndex();

    auto item = static_cast<EventTreeItem*>(sourceIndex.internalPointer());

    if (topLevelType == Event || topLevelType == Month) {
        auto parent = item->getParent();
        if (parent != topLevelItem)
            return QModelIndex();

        return createIndex(sourceIndex.row(), sourceIndex.column(), sourceIndex.internalPointer());
    }
    else if (topLevelType == Year) {
        auto parent = item->getParent();
        auto grandParent = parent->getParent();
        if (grandParent != topLevelItem)
            return QModelIndex();
        else {
            QPair<int, int> p;
            p.first = grandParent->getRow(parent);
            p.second = parent->getRow(item);
            return createIndex(eventMapping[p], sourceIndex.column(), sourceIndex.internalPointer());
        }
    }

    return QModelIndex();
}

QModelIndex PhotoEventListProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!topLevelItem)
        return QModelIndex();

    if (!proxyIndex.isValid())
        return QModelIndex();

    auto pair = getSourceIndex(proxyIndex.row());
    if (topLevelType == Month || topLevelType == Event) {
        return sourceModel()->index(proxyIndex.row(), proxyIndex.column(), topLevelSourceIndex);
    }
    else if (topLevelType == Year) {
        auto monthIndex = sourceModel()->index(pair.first, proxyIndex.column(), topLevelSourceIndex);
        return sourceModel()->index(pair.second, proxyIndex.column(), monthIndex);
    }
    else
        return QModelIndex();
}

QModelIndex PhotoEventListProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (topLevelType == Month || topLevelType == Event)
        return createIndex(row, column, topLevelItem->getChild(row));
    else if (topLevelType == Year) {
        auto pair = getSourceIndex(row);
        return createIndex(row, column, topLevelItem->getChild(pair.first)->getChild(pair.second));
    }

    return QModelIndex();
}

QModelIndex PhotoEventListProxyModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

int PhotoEventListProxyModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid() || !topLevelItem)
        return 0;

    if (topLevelType == Year) {
        int total = 0;
        for (int i = 0; i < topLevelItem->childCount(); i++)
            total += topLevelItem->getChild(i)->childCount();

        return total;
    }
    return topLevelItem->childCount();
}

int PhotoEventListProxyModel::columnCount(const QModelIndex& parent) const
{
    return sourceModel()->columnCount(mapToSource(parent));
}

void PhotoEventListProxyModel::setTopLevelItemFromIndex(const QModelIndex& index)
{
    beginResetModel();
    auto item = static_cast<EventTreeItem*>(index.internalPointer());
    topLevelSourceIndex = QPersistentModelIndex(index);
    setTopLevelItem(item);
    endResetModel();
}

void PhotoEventListProxyModel::setTopLevelItem(EventTreeItem* item)
{
    topLevelItem = item;

    if (dynamic_cast<YearItem*>(topLevelItem))
        topLevelType = Year;
    else if (dynamic_cast<MonthItem*>(topLevelItem))
        topLevelType = Month;
    else if (dynamic_cast<EventItem*>(topLevelItem))
        topLevelType = Event;
    else
        topLevelType = Invalid;

    buildMapping();
}

void PhotoEventListProxyModel::buildMapping()
{
    auto sm = sourceModel();
    int row = 0;

    eventMapping.clear();

    if (topLevelType == Month || topLevelType == Event) {
        for (int i = 0; i < sm->rowCount(topLevelSourceIndex); i++) {
            auto eventIndex = sm->index(i, 0, topLevelSourceIndex);
            eventMapping[{-1, i}] = row;
            row++;
        }
    }
    else if (topLevelType == Year) {
        for (int i = 0; i < sm->rowCount(topLevelSourceIndex); i++) {
            auto monthIndex = sm->index(i, 0, topLevelSourceIndex);
            for (int j = 0; j < sm->rowCount(monthIndex); j++) {
                auto eventIndex = sm->index(j, 0, monthIndex);
                eventMapping[{i, j}] = row;
                row++;
            }
        }
    }
}

QPair<int, int> PhotoEventListProxyModel::getSourceIndex(int row) const
{
    return eventMapping.key(row);
}
