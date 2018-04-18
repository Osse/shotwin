#include "treeproxymodel.h"

#include "photoitem.h"

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFlags>
#include <QStyle>

TreeProxyModel::TreeProxyModel(QObject* parent) : QAbstractProxyModel(parent)
{
    auto style = QApplication::style();
    groupIcon = style->standardPixmap(QStyle::SP_DirClosedIcon);
}

QModelIndex TreeProxyModel::index(int row, int column, const QModelIndex& parent) const
{
    auto c = getCoordinate(parent);
    c.push_back(row);
    return createIndex(c);
}

QModelIndex TreeProxyModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    Coordinate* coord = static_cast<Coordinate*>(child.internalPointer());

    if (!coord)
        return QModelIndex();

    Coordinate parentCoord(coord->begin(), std::prev(coord->end()));

    if (parentCoord.empty())
        return QModelIndex();

    return createIndex(parentCoord);
}

int TreeProxyModel::rowCount(const QModelIndex& parent) const
{
    if (mapToSource(parent).isValid())
        return 0;

    auto p = getCoordinate(parent);
    int rows = 0;
    std::for_each(coordinatesMap.begin(), coordinatesMap.end(), [&rows, p](const auto& c) {
        if (c->size() == p.size() + 1 && std::equal(p.begin(), p.end(), c->begin()))
            rows++;
    });
    return rows;
}

int TreeProxyModel::columnCount(const QModelIndex& parent) const
{
    return rowCount(parent) > 0 ? 1 : 0;
}

QModelIndex TreeProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();

    int row = sourceIndex.row();
    auto coord = coordinates.at(row);
    return getModelIndex(coord);
}

QModelIndex TreeProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    Coordinate c = getCoordinate(proxyIndex);

    if (sourceRows.count(c))
        return sourceModel()->index(sourceRows.at(c), 0);

    return QModelIndex();
}

QVariant TreeProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // QAbstractProxyModel::headerData() calls mapToSource() on the top-level
    // indices of the proxy and uses the sourceIndex's row/column numbers as
    // sections. mapToSource() returns QModelIndex() for those, so just
    // forward the call directly.
    return sourceModel()->headerData(section, orientation, role);
}

Qt::ItemFlags TreeProxyModel::flags(const QModelIndex& index) const
{
    auto sourceFlags = sourceModel()->flags(sourceModel()->index(0, 0));
    sourceFlags.setFlag(Qt::ItemNeverHasChildren, false);
    return sourceFlags;
}

QVariant TreeProxyModel::data(const QModelIndex& proxyIndex, int role) const
{
    auto sourceIndex = mapToSource(proxyIndex);
    if (sourceIndex.isValid()) {
        if (getAlternateDisplayData && role == Qt::DisplayRole)
            return getAlternateDisplayData(sourceModel()->data(sourceIndex, role));
        else
            return sourceModel()->data(sourceIndex, role);
    }
    else if (role == Qt::DecorationRole)
        return groupIcon;
    else if (role == SourceDataRole) {
        auto c = getCoordinate(proxyIndex);
        return sourceData.count(c) ? sourceData.at(c) : QVariant();
    }
    else if (role != Qt::DisplayRole)
        return QVariant();

    return groupData.at(getCoordinate(proxyIndex));
}

void TreeProxyModel::respondToLayoutChanged()
{
    emit layoutAboutToBeChanged();
    refreshMappings();
    emit layoutChanged();
}

void TreeProxyModel::refreshMappings()
{
    auto m = sourceModel();

    clearMappings(m->rowCount());

    std::set<std::vector<QVariant>> seenGroups;

    // At the beginning of each iteration cookie[0, depth + 1]
    // is the coordinate of the most recently inserted node
    Coordinate cookie(20, -1);

    for (int row = 0; row < m->rowCount(); row++) {
        auto groupingData = getGroupingData(m->index(row, 0));
        std::vector<QVariant> sourceDataForIndex;
        if (getSourceData)
            sourceDataForIndex = getSourceData(m->index(row, 0));

        int depth = groupingData.size();

        std::vector<QVariant> newGroup;
        newGroup.reserve(depth);

        for (int i = 0; i < depth; i++) {
            newGroup.push_back(groupingData[i]);

            if (!seenGroups.count(newGroup)) {
                cookie[i]++;
                auto tempEnd = cookie.begin() + i + 1;

                Coordinate temp(cookie.begin(), tempEnd);
                coordinatesMap.insert(std::make_unique<Coordinate>(temp));
                groupData[temp] = groupingData[i];
                if (sourceDataForIndex.size() > i)
                    sourceData[temp] = sourceDataForIndex[i];
                seenGroups.insert(newGroup);

                // New grouping node inserted; reset the last component of the coordinate
                std::fill(tempEnd, cookie.end(), -1);
            }
        }

        cookie[depth]++;

        Coordinate temp(cookie.begin(), cookie.begin() + depth + 1);
        coordinatesMap.insert(std::make_unique<Coordinate>(temp));
        sourceRows[temp] = row;
        coordinates[row] = temp;
    }
}

void TreeProxyModel::clearMappings(int rowCount)
{
    coordinatesMap.clear();
    sourceRows.clear();
    coordinates.clear();
    coordinates.resize(rowCount);
    groupData.clear();
}

TreeProxyModel::Coordinate TreeProxyModel::getCoordinate(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid())
        return {};

    Coordinate* coord = static_cast<Coordinate*>(proxyIndex.internalPointer());
    return Coordinate(*coord);
}

QModelIndex TreeProxyModel::getModelIndex(const TreeProxyModel::Coordinate& coordinate) const
{
    QModelIndex i;
    for (const auto& c : coordinate)
        i = index(c, 0, i);

    return i;
}

QModelIndex TreeProxyModel::createIndex(const Coordinate& coordinate) const
{
    auto it = std::find_if(
        coordinatesMap.begin(), coordinatesMap.end(), [&coordinate](const auto& cptr) { return coordinate == *cptr; });
    if (it == coordinatesMap.end())
        return QModelIndex();
    else
        return createIndex(coordinate.back(), 0, static_cast<void*>(it->get()));
}

void TreeProxyModel::setAlternateDisplayDataCb(const AlternateDisplayDataCbType& value)
{
    getAlternateDisplayData = value;
}

void TreeProxyModel::setGroupingDataCb(const GroupingDataCbType& value)
{
    getGroupingData = value;
}

void TreeProxyModel::setSourceDataCb(const GroupingDataCbType& value)
{
    getSourceData = value;
}

void TreeProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    beginResetModel();
    QAbstractProxyModel::setSourceModel(sourceModel);
    refreshMappings();
    endResetModel();
    connect(sourceModel, &QAbstractItemModel::layoutChanged, this, &TreeProxyModel::respondToLayoutChanged);
}
