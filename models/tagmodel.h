#ifndef TAGMODEL_H
#define TAGMODEL_H

#include <QAbstractListModel>
#include <QPixmap>

#include <map>

class TagModel : public QAbstractListModel
{
    Q_OBJECT
public:
    TagModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    enum UserRoles { PhotoListRole = Qt::UserRole + 1 };

    struct TagItem {
        int id;
        QString name;
        std::vector<int> photos;
    };

    QStringList getTagsForPhoto(int id) const;
    std::vector<int> getAllTaggedPhotos() const;

private:
    void init();

    std::vector<TagItem> tags;
    std::multimap<int, QString> idToTags;
    QPixmap icon{":/icons/one-tag.png"};
};

#endif  // TAGMODEL_H
