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

    QModelIndex index(int row, int column, const QModelIndex& parent) const override;

    virtual QHash<int, QByteArray> roleNames() const;
    enum UserRoles {
        ThumnailRole = Qt::UserRole + 1,
        FilenameRole,
        ExposureTimeRole,
        PhotoIdRole,
        EventIdRole,
        TypeRole
    };

private:
    QString mappedFile(const QString& file);
    void init();

private:
    std::vector<PhotoItem> photoList;
};

#endif  // PHOTOMODEL_H
