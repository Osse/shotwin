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

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;

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
