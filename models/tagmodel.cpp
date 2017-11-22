#include "tagmodel.h"

#include <QApplication>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QStyle>

const QString tagListQuery("select * from TagTable");

TagModel::TagModel(QObject* parent) : QAbstractListModel(parent)
{
    auto style = QApplication::style();
    icon = style->standardPixmap(QStyle::SP_DriveDVDIcon);
    init();
}

int TagModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return tags.size();
}

QVariant TagModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (row >= tags.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return tags[row].name;
    if (role == Qt::DecorationRole)
        return icon;

    return QVariant();
}

QModelIndex TagModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || column > 0)
        return QModelIndex();

    return createIndex(row, column, const_cast<TagItem*>(&tags[row]));
}

QVariant TagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0 || orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole)
        return QVariant("Tags");

    if (role == Qt::FontRole) {
        auto font = QAbstractItemModel::headerData(section, orientation, Qt::FontRole).value<QFont>();
        font.setBold(true);
        return font;
    }

    return QVariant();
}

QStringList TagModel::getTagsForPhoto(int id)
{
    QStringList tags;
    auto startEnd = idToTags.equal_range(id);
    std::multimap<int, QString>::const_iterator it = startEnd.first;
    while (it != startEnd.second) {
        tags << it->second;
        it++;
    }

    return tags;
}

void TagModel::init()
{
    auto db = QSqlDatabase::database();

    QSqlQuery query(tagListQuery, db);

    if (!query.exec())
        return;

    while (query.next()) {
        auto id = query.value("id").toInt();
        auto name = query.value("name").toString();
        auto photo_id_list = query.value("photo_id_list").toString().split(",", QString::SkipEmptyParts);
        std::vector<int> photos;
        photos.reserve(photo_id_list.size());

        for (const auto& idString : photo_id_list) {
            int id = idString.right(idString.size() - idString.indexOf("0")).toInt(nullptr, 16);
            photos.push_back(id);
            idToTags.insert({id, name});
        }

        tags.push_back(TagItem{id, name, photos});
    }
}
