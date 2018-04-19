#ifndef TREEPROXYMODEL_H
#define TREEPROXYMODEL_H

#include "tree.hh"

#include <QAbstractProxyModel>
#include <QPixmap>
#include <QString>

#include <functional>
#include <memory>
#include <set>

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
    void setGroupingDataCb(const GroupingDataCbType& value);
    void setSourceDataCb(const GroupingDataCbType& value);
    using AlternateDisplayDataCbType = std::function<QVariant(const QVariant& value)>;
    void setAlternateDisplayDataCb(const AlternateDisplayDataCbType& value);

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    enum UserRoles { SourceDataRole = Qt::UserRole + 0x200 };

private:
    void refreshMappings();
    void respondToLayoutChanged();

private:
    GroupingDataCbType getGroupingData;
    GroupingDataCbType getSourceData;
    AlternateDisplayDataCbType getAlternateDisplayData;

    tree<int> sourceRows;
    tree<QVariant> nodeShit;

    QPixmap groupIcon;
};

#endif  // TREEPROXYMODEL_H
