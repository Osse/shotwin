#include "eventtreemodel.h"

EventTreeModel::EventTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
}

EventTreeModel::~EventTreeModel()
{
}

QModelIndex EventTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    return QModelIndex();
}

QModelIndex EventTreeModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}

int EventTreeModel::rowCount(const QModelIndex& parent) const
{
    return 0;
}

QVariant EventTreeModel::data(const QModelIndex& index, int role) const
{
    return QVariant();
}

int EventTreeModel::columnCount(const QModelIndex& parent) const
{
    return 0;
}
