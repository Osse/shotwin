#ifndef EVENTTREEMODEL_H
#define EVENTTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>

class EventTreeModel : public QAbstractItemModel
{
public:
    EventTreeModel(QObject* parent = nullptr);
    ~EventTreeModel();
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
};

#endif  // EVENTTREEMODEL_H
