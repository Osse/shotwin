#ifndef EVENTTREEMODEL_H
#define EVENTTREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>

class EventTreeItem;

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
    virtual QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const;
    virtual QHash<int, QByteArray> roleNames() const;
    void init();

private:
    void sort();
    enum UserRoles { ThumnailRole = Qt::UserRole + 1, FilenameRole, TimespanRole, ChildrenCountRole };
    EventTreeItem* getItem(const QModelIndex& index) const;

private:
    EventTreeItem* rootItem;
    QString mappedFile(const QString& file);
};

#endif  // EVENTTREEMODEL_H
