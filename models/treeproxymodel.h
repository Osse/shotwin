#ifndef TREEPROXYMODEL_H
#define TREEPROXYMODEL_H

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

    void setSourceModel(QAbstractItemModel* sourceModel) override;

    enum UserRoles { SourceDataRole = Qt::UserRole + 0x200 };

private:
    using Coordinate = std::vector<int>;
    using CoordinatePtr = std::unique_ptr<std::vector<int>>;

    void refreshMappings();
    void clearMappings(int rowCount);
    void respondToLayoutChanged();
    Coordinate getCoordinate(const QModelIndex& proxyIndex) const;
    QModelIndex getModelIndex(const Coordinate& coordinate) const;

    using QAbstractProxyModel::createIndex;
    QModelIndex createIndex(int row, const CoordinatePtr& coordinate) const;

private:
    GroupingDataCbType getGroupingData;

    // All coordinates
    std::map<Coordinate, CoordinatePtr> coordinatesMap;

    // mapping to source
    std::map<Coordinate, int> sourceRows;

    // mapping from source (index is source row)
    std::vector<Coordinate> coordinates;

    // Data for extra nodes
    std::map<Coordinate, QVariant> displayData;
    std::map<Coordinate, QVariant> sourceData;

    QPixmap groupIcon;
};

#endif  // TREEPROXYMODEL_H
