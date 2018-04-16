#include "treeproxymodel.h"

#include "photoitem.h"

#include <QApplication>
#include <QDate>
#include <QDebug>
#include <QFile>
#include <QFlags>
#include <QStyle>

#include <functional>
#include <iostream>

TreeProxyModel::TreeProxyModel(QObject* parent) : QAbstractProxyModel(parent)
{
    auto style = QApplication::style();
    groupIcon = style->standardPixmap(QStyle::SP_DirClosedIcon);
    comp = [](auto v1, auto v2) { return v1 < v2; };
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
    wrapper();
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

enum Kek {
    Equal = -1,
    FirstShorter = -2,
    SecondShorter = -3,
};

int getDiff(const std::vector<QVariant>& v1, const std::vector<QVariant>& v2)
{
    auto elems = std::mismatch(v1.begin(), v1.end(), v2.begin(), v2.end());

    if (elems.first == v1.end() && elems.second == v2.end())
        return Kek::Equal;
    else if (elems.first == v1.end())
        return Kek::FirstShorter;
    else if (elems.second == v2.end())
        return Kek::SecondShorter;
    else
        return (int)std::distance(v1.begin(), elems.first);
}

void TreeProxyModel::refreshMappings2()
{
    qDebug() << "kek";
    auto m = sourceModel();

    clearMappings(m->rowCount());

    std::map<std::vector<QVariant>, std::vector<int>, GroupingDataCompCbType> groupToRows(comp);

    for (int row = 0; row < m->rowCount(); ++row) {
        auto groupingData = getGroupingData(m->index(row, 0));
        groupToRows[groupingData].push_back(row);
    }

    std::vector<QVariant> prevGroupingData;
    std::vector<int> prevRows;
    Coordinate prevCoord;

    for (const auto& kv : groupToRows) {
        const auto& groupingData = kv.first;
        const auto& rows = kv.second;

        int n = getDiff(prevGroupingData, groupingData);

        if (n == Kek::Equal) {
            Coordinate coord = prevCoord;
            coord.push_back(-1);
            for (const auto& row : rows) {
                coord.back()++;
                coordinates[row] = coord;
                if (sourceRows.count(coord))
                    qDebug() << "OOOOMG";
                sourceRows[coord] = row;
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            }
        }
        else if (n == Kek::FirstShorter) {
            // Reuse previous coordinate
            Coordinate coord = prevCoord;
            for (int i = coord.size(); i < groupingData.size(); ++i) {
                coord.push_back(0);
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
                groupData[coord] = groupingData[i];
            }

            prevCoord = coord;
            coord.push_back(-1);
            for (const auto& row : rows) {
                coord.back()++;
                coordinates[row] = coord;
                if (sourceRows.count(coord))
                    qDebug() << "OOOOMG";
                sourceRows[coord] = row;
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            }
        }
        else if (n == Kek::SecondShorter) {
            // Reuse parts of previous coordinate
            Coordinate coord(prevCoord.begin(), prevCoord.begin() + groupingData.size());
            prevCoord = coord;
            coord.push_back(-1);
            for (const auto& row : rows) {
                coord.back()++;
                coordinates[row] = coord;
                if (sourceRows.count(coord))
                    qDebug() << "OOOOMG";
                sourceRows[coord] = row;
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            }
        }
        else if (n == 0) {
            Coordinate coord(1);
            coord[0] = prevCoord[0] + 1;
            groupData[coord] = groupingData[0];
            coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            for (int i = coord.size(); i < groupingData.size(); ++i) {
                coord.push_back(0);
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
                groupData[coord] = groupingData[i];
            }
            prevCoord = coord;
            coord.push_back(-1);
            for (const auto& row : rows) {
                coord.back()++;
                coordinates[row] = coord;
                if (sourceRows.count(coord))
                    qDebug() << "OOOOMG";
                sourceRows[coord] = row;
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            }
        }
        else {
            // Reuse parts of previous coordinate
            Coordinate coord(prevCoord.begin(), prevCoord.begin() + n + 1);
            coord[n]++;
            groupData[coord] = groupingData[n];
            coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            for (int i = coord.size(); i < groupingData.size(); ++i) {
                coord.push_back(0);
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
                groupData[coord] = groupingData[i];
            }

            prevCoord = coord;
            coord.push_back(-1);
            for (const auto& row : rows) {
                coord.back()++;
                coordinates[row] = coord;
                if (sourceRows.count(coord))
                    qDebug() << "OOOOMG";
                sourceRows[coord] = row;
                coordinatesMap.insert(std::make_unique<Coordinate>(coord));
            }
        }

        prevGroupingData = groupingData;
        prevRows = rows;
    }
}

void TreeProxyModel::dump(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);
    auto m = sourceModel();

    for (const auto& kv : sourceRows) {
        const auto& coord = kv.first;
        const auto& row = kv.second;

        out << "{ ";
        for (int i = 1; i < coord.size(); ++i) {
            out << groupData[{coord.begin(), coord.begin() + i}].toString() << " ";
        }
        out << "} -> ";
        for (int i : coord)
            out << i << " ";
        out << "\n";
    }

    //    for (int row = 0; row < m->rowCount(); ++row) {
    //        out << row << ": { ";
    //        auto c = coordinates[row];
    //        for (int i : c)
    //            out << i << " ";
    //        out << "}\n";

    //        out << "{ ";
    //        for (const auto& v : getGroupingData(m->index(row, 0)))
    //            out << v.toString() << " ";
    //        out << "}\n";
    //    }
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

void TreeProxyModel::setComp(const GroupingDataCompCbType& value)
{
    qDebug() << "new comp";
    comp = value;
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
    wrapper();
    endResetModel();
    connect(sourceModel, &QAbstractItemModel::layoutChanged, this, &TreeProxyModel::respondToLayoutChanged);
}

void TreeProxyModel::wrapper()
{
    qDebug() << "kek";
    //    refreshMappings();
    //    qDebug() << "coordinates: " << coordinatesMap.size();
    //    qDebug() << "sourceRows: " << sourceRows.size();

    //    std::vector<int> rows1;
    //    for (const auto& kv : sourceRows)
    //        rows1.push_back(kv.second);
    //    std::sort(rows1.begin(), rows1.end());

    refreshMappings2();
    //    qDebug() << "coordinates: " << coordinatesMap.size();
    //    qDebug() << "sourceRows: " << sourceRows.size();

    //    std::vector<int> rows2;
    //    for (const auto& kv : sourceRows)
    //        rows2.push_back(kv.second);
    //    std::sort(rows2.begin(), rows2.end());

    //    decltype(rows1) diff;

    //    std::set_difference(rows1.begin(), rows1.end(), rows2.begin(), rows2.end(), std::inserter(diff,
    //    diff.begin()));

    //    qDebug() << "diff: " << diff.size();
    //    qDebug() << "diff: " << diff.size();
}
