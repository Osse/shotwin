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
    Coordinate c;
    if (parent.isValid())
        c = getCoordinate(parent);

    c.push_back(row);
    auto it = std::find_if(coordinatesMap.begin(), coordinatesMap.end(), [&c](const auto& cptr) { return c == *cptr; });
    if (it == coordinatesMap.end())
        return QModelIndex();
    else
        return createIndex(row, *it);
}

QModelIndex TreeProxyModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    Coordinate* coord = static_cast<Coordinate*>(child.internalPointer());

    if (!coord)
        return QModelIndex();

    Coordinate parentCoord(coord->begin(), coord->begin() + coord->size() - 1);

    if (parentCoord.empty())
        return QModelIndex();

    auto it = std::find_if(coordinatesMap.begin(), coordinatesMap.end(), [&parentCoord](const auto& cptr) {
        return parentCoord == *cptr;
    });

    return createIndex(parentCoord.back(), *it);
}

int TreeProxyModel::rowCount(const QModelIndex& parent) const
{
    Coordinate p;
    if (parent.isValid())
        p = getCoordinate(parent);

    int rows = 0;
    std::for_each(coordinatesMap.begin(), coordinatesMap.end(), [&rows, p](const auto& c) {
        if (c->size() == p.size() + 1 && std::equal(p.begin(), p.end(), c->begin()))
            rows++;
    });
    return rows;
}

int TreeProxyModel::columnCount(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return 1;

    Coordinate c = getCoordinate(parent);

    int sourceDepth = coordinates[0].size();

    return c.size() == sourceDepth ? 0 : 1;
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
    if (sourceIndex.isValid())
        return sourceModel()->data(sourceIndex, role);
    else if (role == Qt::DecorationRole)
        return groupIcon;
    else if (role == SourceDataRole)
        return sourceData.at(getCoordinate(proxyIndex));
    else if (role != Qt::DisplayRole)
        return QVariant();

    return displayData.at(getCoordinate(proxyIndex));
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

    std::set<int> seenYears;
    std::set<std::pair<int, int>> seenMonths;

    Coordinate coordinate{-1, -1, -1};
    for (int row = 0; row < m->rowCount(); row++) {
        auto groupingData = getGroupingData(m->index(row, 0));

        int year = groupingData[0].first;
        int month = groupingData[1].first;

        if (!seenYears.count(year)) {
            coordinate[0]++;
            coordinate[1] = -1;
            coordinate[2] = -1;
            Coordinate temp(1, coordinate[0]);
            // coordinatesMap[temp] = std::make_unique<Coordinate>(temp);
            coordinatesMap.emplace(std::make_unique<Coordinate>(temp));
            displayData[temp] = groupingData[0].second;
            sourceData[temp] = year;
            seenYears.insert(year);
        }

        if (!seenMonths.count({year, month})) {
            coordinate[1]++;
            coordinate[2] = -1;
            Coordinate temp{coordinate[0], coordinate[1]};
            // coordinatesMap[temp] = std::make_unique<Coordinate>(temp);
            coordinatesMap.emplace(std::make_unique<Coordinate>(temp));
            displayData[temp] = groupingData[1].second;
            sourceData[temp] = month;
            seenMonths.insert({year, month});
        }

        coordinate[2]++;

        sourceRows[coordinate] = row;
        coordinates[row] = coordinate;
        // coordinatesMap[coordinate] = std::make_unique<Coordinate>(coordinate);
        coordinatesMap.insert(std::make_unique<Coordinate>(coordinate));
    }
}

void TreeProxyModel::clearMappings(int rowCount)
{
    coordinatesMap.clear();
    sourceRows.clear();
    coordinates.clear();
    coordinates.resize(rowCount);
    displayData.clear();
    sourceData.clear();
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

QModelIndex TreeProxyModel::createIndex(int row, const CoordinatePtr& coordinate) const
{
    return createIndex(row, 0, static_cast<void*>(coordinate.get()));
}

void TreeProxyModel::setGroupingDataCb(const GroupingDataCbType& value)
{
    getGroupingData = value;
}

void TreeProxyModel::setSourceModel(QAbstractItemModel* sourceModel)
{
    beginResetModel();
    QAbstractProxyModel::setSourceModel(sourceModel);
    refreshMappings();
    endResetModel();
    connect(sourceModel, &QAbstractItemModel::layoutChanged, this, &TreeProxyModel::respondToLayoutChanged);
}
