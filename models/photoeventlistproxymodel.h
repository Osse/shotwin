#ifndef PHOTOEVENTLISTPROXYMODEL_H
#define PHOTOEVENTLISTPROXYMODEL_H

#include "eventtreeitem.h"

#include <QAbstractProxyModel>
#include <QMap>
#include <QPair>
#include <QPersistentModelIndex>

class PhotoEventListProxyModel : public QAbstractProxyModel
{
public:
    PhotoEventListProxyModel(QObject* parent = nullptr);

    virtual QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;
    virtual QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
    virtual QModelIndex index(int row, int column, const QModelIndex& parent) const;
    virtual QModelIndex parent(const QModelIndex& child) const;
    virtual int rowCount(const QModelIndex& parent) const;
    virtual int columnCount(const QModelIndex& parent) const;

    void setTopLevelItemFromIndex(const QModelIndex& index);
    void setTopLevelItem(EventTreeItem* item);

private:
    QPair<int, int> getSourceIndex(int row) const;
    void buildMapping();

private:
    enum TopLevelType { Year, Month, Event, Invalid };
    TopLevelType topLevelType = Invalid;
    EventTreeItem* topLevelItem = nullptr;
    QPersistentModelIndex topLevelSourceIndex;
    QMap<QPair<int, int>, int> eventMapping;
};

#endif /* end of include guard: PHOTOEVENTLISTPROXYMODEL_H */
