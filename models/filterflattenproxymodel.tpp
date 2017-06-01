#include "filterflattenproxymodel.h"

#include "PhotoItem.h"
#include "eventitem.h"

#include <QDebug>

template <class ItemType>
FilterFlattenProxyModel<ItemType>::FilterFlattenProxyModel(QObject* parent) : QAbstractProxyModel(parent)
{
}

template <class ItemType>
FilterFlattenProxyModel<ItemType>::~FilterFlattenProxyModel()
{
}

template <class ItemType>
QModelIndex FilterFlattenProxyModel<ItemType>::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || !hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column, mapping.key(row).internalPointer());
}

template <class ItemType>
QModelIndex FilterFlattenProxyModel<ItemType>::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

template <class ItemType>
int FilterFlattenProxyModel<ItemType>::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return mapping.size();
}

template <class ItemType>
int FilterFlattenProxyModel<ItemType>::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return sourceModel()->columnCount(mapToSource(parent));
}

template <class ItemType>
QModelIndex FilterFlattenProxyModel<ItemType>::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid())
        return QModelIndex();

    auto sourceIndex = mapping.key(proxyIndex.row(), QModelIndex());

    return sourceIndex;
}

template <class ItemType>
QModelIndex FilterFlattenProxyModel<ItemType>::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();

    return createIndex(mapping[sourceIndex], sourceIndex.column(), sourceIndex.internalPointer());
}

template <class ItemType>
void FilterFlattenProxyModel<ItemType>::setTopLevelIndex(const QModelIndex& index)
{
    beginResetModel();
    topLevelSourceIndex = QPersistentModelIndex(index);
    mapping.clear();
    buildMapping();
    endResetModel();
}

template <class ItemType>
void FilterFlattenProxyModel<ItemType>::buildMapping()
{
    int row = 0;
    buildMapping(topLevelSourceIndex, row);
}
