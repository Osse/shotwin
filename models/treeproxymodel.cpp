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
    if (column > 0)
        return {};

    tree<TreeProxyModel::Data>::iterator it;
    if (parent.isValid()) {
        auto pit = getIterator(parent);
        it = nodes.child(pit, row);
    }
    else
        it = nodes.child(nodes.begin(), row);

    return createIndex(row, column, it.node);
}

QModelIndex TreeProxyModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return {};

    auto cit = getIterator(child);
    auto pit = nodes.parent(cit);

    if (pit == nodes.begin())
        return {};

    return createIndex(nodes.index(pit), 0, pit.node);
}

int TreeProxyModel::rowCount(const QModelIndex& parent) const
{
    tree<TreeProxyModel::Data>::iterator it = parent.isValid() ? getIterator(parent) : nodes.begin();
    return it.number_of_children();
}

int TreeProxyModel::columnCount(const QModelIndex& parent) const
{
    return rowCount(parent) > 0 ? 1 : 0;
}

QModelIndex TreeProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    if (!sourceIndex.isValid())
        return {};

    auto res = std::find_if(nodes.begin_leaf(), nodes.end_leaf(), [row = sourceIndex.row()](const Data& d) {
        return std::get<int>(d) == row;
    });

    if (res == nodes.end_leaf())
        return {};

    return createIndex(nodes.index(res), sourceIndex.column(), res.node);
}

QModelIndex TreeProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    if (!proxyIndex.isValid() || rowCount(proxyIndex) > 0)
        return QModelIndex();

    int row = sourceRow(proxyIndex);
    return sourceModel()->index(row, 0);
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
        return getSourceDataForIndex(proxyIndex);
    }
    else if (role == Qt::DisplayRole)
        return groupData(proxyIndex);

    return QVariant();
}

void TreeProxyModel::respondToLayoutChanged()
{
    emit layoutAboutToBeChanged();
    refreshMappings();
    emit layoutChanged();
}

tree<TreeProxyModel::Data>::iterator TreeProxyModel::getIterator(const QModelIndex& proxyIndex) const
{
    using Node = tree_node_<decltype(nodes)::value_type>;
    auto x = static_cast<Node*>(proxyIndex.internalPointer());
    return tree<TreeProxyModel::Data>::iterator(x);
}

QVariant TreeProxyModel::groupData(const tree<TreeProxyModel::Data>::iterator& it) const
{
    return std::get<0>(*it).first;
}

QVariant TreeProxyModel::sourceData(const tree<TreeProxyModel::Data>::iterator& it) const
{
    return std::get<0>(*it).second;
}

int TreeProxyModel::sourceRow(const tree<TreeProxyModel::Data>::iterator& it) const
{
    return std::get<1>(*it);
}

QVariant TreeProxyModel::groupData(const QModelIndex& proxyIndex) const
{
    return groupData(getIterator(proxyIndex));
}

QVariant TreeProxyModel::sourceData(const QModelIndex& proxyIndex) const
{
    return sourceData(getIterator(proxyIndex));
}

int TreeProxyModel::sourceRow(const QModelIndex& proxyIndex) const
{
    return sourceRow(getIterator(proxyIndex));
}

QVariant TreeProxyModel::getSourceDataForIndex(const QModelIndex& proxyIndex) const
{
    auto pit = getIterator(proxyIndex);
    auto it(pit);
    while (it.number_of_children() > 0)
        it = it.begin();
    auto sourceData = getSourceData2(sourceModel()->index(sourceRow(it), 0)).value<QList<int>>();
    sourceData = sourceData.mid(0, nodes.depth(pit));
    return QVariant::fromValue(sourceData);
}

void TreeProxyModel::setGetSourceData2(const GroupingDataCbType2& value)
{
    getSourceData2 = value;
}

void TreeProxyModel::refreshMappings()
{
    auto m = sourceModel();

    clearMappings();

    std::map<std::vector<QVariant>, std::vector<int>> groupsToRows;
    std::map<std::vector<QVariant>, std::vector<QVariant>> groupsToSource;

    for (int row = 0; row < m->rowCount(); ++row) {
        auto groupingData = getGroupingData(m->index(row, 0));
        groupsToRows[groupingData].push_back(row);
        if (groupsToSource.count(groupingData) == 0)
            groupsToSource[groupingData] = getSourceData(m->index(row, 0));
    }

    for (const auto& kv : groupsToRows) {
        const auto groupingData = kv.first;
        const auto& rows = kv.second;

        tree<TreeProxyModel::Data>::sibling_iterator it(nodes.begin());
        int i = 0;
        for (const auto& g : groupingData) {
            auto res =
                std::find_if(nodes.begin(it), nodes.end(it), [g](const Data& d) { return std::get<0>(d).first == g; });
            if (res != nodes.end(it))
                it = res;
            else
                it = nodes.append_child(it, Data{std::in_place_index<0>, g, groupsToSource[groupingData][i]});
            i++;
        }
        for (const auto& r : rows)
            nodes.append_child(it, r);
    }
}

void TreeProxyModel::clearMappings()
{
    nodes.clear();
    nodes.set_head({});
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
    if (!getSourceData)
        getSourceData = getGroupingData;
    refreshMappings();
    endResetModel();
    connect(sourceModel, &QAbstractItemModel::layoutChanged, this, &TreeProxyModel::respondToLayoutChanged);
}
