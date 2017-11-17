#include "photomodel.h"

#include <QDateTime>
#include <QFont>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>

const QString photoListQuery(
    "select id, event_id, exposure_time, filename, type from PhotoVideoView order by exposure_time desc;");

PhotoModel::PhotoModel(QObject* parent) : QAbstractListModel(parent)
{
    map = QSettings().value("map").toMap();
    init();
}

PhotoModel::~PhotoModel()
{
}

int PhotoModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return photoList.size();
}

QVariant PhotoModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (row >= photoList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return photoList[row].displayString();
    else if (role == ThumnailRole)
        return photoList[row].getThumbnailId();
    else if (role == FilenameRole)
        return photoList[row].getFilename();
    else if (role == MappedFilenameRole)
        return mappedFile(photoList[row].getFilename());
    else if (role == PhotoIdRole)
        return photoList[row].getPhotoId();
    else if (role == EventIdRole)
        return photoList[row].getEventId();
    else if (role == ExposureTimeRole)
        return photoList[row].getExposureTime();
    else if (role == TypeRole)
        return photoList[row].getType();
    else
        return QVariant();
}

QModelIndex PhotoModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || column > 0)
        return QModelIndex();

    return createIndex(row, column, const_cast<PhotoItem*>(&photoList[row]));
}

QList<QModelIndex> PhotoModel::match(
    const QModelIndex& start, int role, const QVariant& value, int hits, Qt::MatchFlags flags) const
{
    if (role == PhotoIdRole) {
        int id = value.toInt();
        return {index(idPhotoMap.at(id), 0)};
    }
    else
        return QAbstractListModel::match(start, role, value, hits, flags);
}

QHash<int, QByteArray> PhotoModel::roleNames() const
{
    auto roleNames = QAbstractItemModel::roleNames();
    roleNames[ThumnailRole] = "thumbnail";
    roleNames[FilenameRole] = "filename";
    roleNames[MappedFilenameRole] = "mappedfilename";
    roleNames[EventIdRole] = "eventid";
    roleNames[PhotoIdRole] = "id";
    roleNames[TypeRole] = "type";
    return roleNames;
}

QVariant PhotoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0 || orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole)
        return QVariant("Photos");

    if (role == Qt::FontRole) {
        auto font = QAbstractItemModel::headerData(section, orientation, Qt::FontRole).value<QFont>();
        font.setBold(true);
        return font;
    }

    return QVariant();
}

void PhotoModel::sort(int column, Qt::SortOrder order)
{
    if (column != 0)
        return;

    if (order == Qt::AscendingOrder)
        std::sort(photoList.begin(), photoList.end(), [](const auto& p1, const auto& p2) {
            return p1.getExposureTime() < p2.getExposureTime();
        });
    else
        std::sort(photoList.begin(), photoList.end(), [](const auto& p1, const auto& p2) {
            return p1.getExposureTime() > p2.getExposureTime();
        });
}

QString PhotoModel::mappedFile(const QString& file) const
{
    QString fileName(file);
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (fileName.startsWith(it.key())) {
            fileName.replace(it.key(), it.value().toString());
            return fileName;
        }
    }
    return QString();
}

void PhotoModel::init()
{
    auto db = QSqlDatabase::database();

    QSqlQuery query(photoListQuery, db);

    if (!query.exec())
        return;

    while (query.next()) {
        int photoId = query.value("id").toInt();
        int eventId = query.value("event_id").toInt();
        auto exposureTime = QDateTime::fromSecsSinceEpoch(query.value("exposure_time").toInt());
        QString fileName = query.value("filename").toString();
        QString type = query.value("type").toString();

        photoList.push_back(PhotoItem(photoId, eventId, exposureTime, fileName, type));
        idPhotoMap[photoId] = photoList.size() - 1;
    }

    sort(0, Qt::DescendingOrder);
}
