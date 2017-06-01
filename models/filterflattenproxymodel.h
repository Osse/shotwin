#ifndef FILTERFLATTENPROXYMODEL_H
#define FILTERFLATTENPROXYMODEL_H

#include <QAbstractProxyModel>

class EventTreeItem;

template <class ItemType>
class FilterFlattenProxyModel : public QAbstractProxyModel
{
public:
    FilterFlattenProxyModel(QObject* parent = nullptr);
    ~FilterFlattenProxyModel();

    QModelIndex index(int row, int column, const QModelIndex& parent) const;
    QModelIndex parent(const QModelIndex& child) const;
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const;

    void setTopLevelIndex(const QModelIndex& index);

private:
    void buildMapping();
    void buildMapping(const QModelIndex& parentIndex, int& row)
    {
        auto sm = sourceModel();
        for (int i = 0; i < sm->rowCount(parentIndex); ++i) {
            auto childIndex = sm->index(i, 0, parentIndex);
            auto item = static_cast<EventTreeItem*>(childIndex.internalPointer());
            auto itemOfType = dynamic_cast<ItemType*>(item);
            if (itemOfType) {
                mapping[childIndex] = row;
                ++row;
            }
            buildMapping(childIndex, row);
        }
    }

private:
    QPersistentModelIndex topLevelSourceIndex;
    QMap<QPersistentModelIndex, int> mapping;
};

#include "filterflattenproxymodel.tpp"

#endif  // FILTERFLATTENPROXYMODEL_H
