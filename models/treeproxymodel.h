#ifndef TREEPROXYMODEL_H
#define TREEPROXYMODEL_H

#include "tree.hh"

#include <QAbstractProxyModel>
#include <QPixmap>
#include <QString>

#include <functional>
#include <memory>
#include <set>
#include <variant>

class TreeProxyModel : public QAbstractProxyModel
{
public:
    TreeProxyModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& child) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& proxyIndex, int role = Qt::DisplayRole) const override;

    QModelIndex mapFromSource(const QModelIndex& sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex& proxyIndex) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    using GroupingDataCbType = std::function<std::vector<QVariant>(const QModelIndex&)>;
    using GroupingDataCbType2 = std::function<QVariant(const QModelIndex&)>;
    void setGroupingDataCb(const GroupingDataCbType& value);
    void setSourceDataCb(const GroupingDataCbType& value);
    void setGetSourceData2(const GroupingDataCbType2& value);
    void setSourceDataCovertCb(const GroupingDataCbType& value);
    using AlternateDisplayDataCbType = std::function<QVariant(const QVariant& value)>;
    void setAlternateDisplayDataCb(const AlternateDisplayDataCbType& value);

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    enum UserRoles { SourceDataRole = Qt::UserRole + 0x200 };

    template <class T>
    void setGetSourceData3(const GroupingDataCbType2& value);

private:
    using Data = std::variant<std::pair<QVariant, QVariant>, int>;
    void refreshMappings();
    void clearMappings();
    void respondToLayoutChanged();
    tree<Data>::iterator getIterator(const QModelIndex& proxyIndex) const;
    QVariant groupData(const tree<TreeProxyModel::Data>::iterator& it) const;
    QVariant sourceData(const tree<TreeProxyModel::Data>::iterator& it) const;
    int sourceRow(const tree<TreeProxyModel::Data>::iterator& it) const;
    QVariant groupData(const QModelIndex& proxyIndex) const;
    QVariant sourceData(const QModelIndex& proxyIndex) const;
    int sourceRow(const QModelIndex& proxyIndex) const;
    QVariant getSourceDataForIndex(const QModelIndex& proxyIndex) const;

private:
    GroupingDataCbType getGroupingData;
    GroupingDataCbType getSourceData;
    GroupingDataCbType2 getSourceData2;
    GroupingDataCbType2 getSourceData3;
    AlternateDisplayDataCbType getAlternateDisplayData;

    tree<Data> nodes;
    QPixmap groupIcon;
};

template <class T>
void TreeProxyModel::setGetSourceData3(const GroupingDataCbType2& value)
{
    getSourceData3 = value;
}

#endif  // TREEPROXYMODEL_H
