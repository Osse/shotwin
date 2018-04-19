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
    return {};
}

QModelIndex TreeProxyModel::parent(const QModelIndex& child) const
{
    return {};
}

int TreeProxyModel::rowCount(const QModelIndex& parent) const
{
    return {};
}

int TreeProxyModel::columnCount(const QModelIndex& parent) const
{
    return rowCount(parent) > 0 ? 1 : 0;
}

QModelIndex TreeProxyModel::mapFromSource(const QModelIndex& sourceIndex) const
{
    return {};
}

QModelIndex TreeProxyModel::mapToSource(const QModelIndex& proxyIndex) const
{
    return {};
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
    return {};
}

void TreeProxyModel::respondToLayoutChanged()
{
    emit layoutAboutToBeChanged();
    refreshMappings();
    emit layoutChanged();
}

void TreeProxyModel::refreshMappings()
{
    //    auto m = sourceModel();

    //    std::map<GroupingData, int> groups;

    //    for (int row = 0; row < m->rowCount(); ++row) {
    //        auto groupingData = getGroupingData(m->index(row, 0));
    //        tree<QVariant>::fixed_depth_iterator it = nodeShit.begin();
    //        for (const auto& g : groupingData) {
    //            it = std::find(nodeShit.begin_fixed(it, 0), nodeShit.end_fixed(it, 0), g);
    //            if (it != nodeShit.end())
    //                continue;
    //        }
    //    }
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
