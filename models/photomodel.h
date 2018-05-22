#ifndef PHOTOMODEL_H
#define PHOTOMODEL_H

#include <QAbstractListModel>

#include "photoitem.h"

#include <map>
#include <vector>

class PhotoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PhotoModel(QMap<QString, QVariant> map, QObject* parent = nullptr);
    ~PhotoModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QList<QModelIndex> match(const QModelIndex& start,
                             int role,
                             const QVariant& value,
                             int hits = 1,
                             Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith | Qt::MatchWrap)) const override;

    QHash<int, QByteArray> roleNames() const;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    enum UserRoles {
        ThumbnailRole = Qt::UserRole + 1,
        FilenameRole,
        MappedFilenameRole,
        ExposureTimeRole,
        PhotoIdRole,
        EventIdRole,
        TypeRole,
        RatingRole,
    };

    QMap<QString, QVariant> getMap() const;
    void setMap(const QMap<QString, QVariant>& value);

private:
    QString mappedFile(const QString& file) const;
    void init();

private:
    std::vector<PhotoItem> photoList;
    std::map<int, size_t> idPhotoMap;
    QMap<QString, QVariant> map;
};

#endif  // PHOTOMODEL_H
