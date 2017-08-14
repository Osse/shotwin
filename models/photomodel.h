#ifndef PHOTOMODEL_H
#define PHOTOMODEL_H

#include <QAbstractListModel>

#include "photoitem.h"

#include <vector>

class PhotoModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PhotoModel(QObject* parent = nullptr);
    ~PhotoModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const;

    enum UserRoles {
        ThumnailRole = Qt::UserRole + 1,
        FilenameRole,
        MappedFilenameRole,
        ExposureTimeRole,
        PhotoIdRole,
        EventIdRole,
    };

private:
    QString mappedFile(const QString& file) const;
    void init();

private:
    std::vector<PhotoItem> photoList;
    QMap<QString, QVariant> map;
};

#endif  // PHOTOMODEL_H
