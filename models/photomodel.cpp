#include "photomodel.h"

#include <QDateTime>
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

QHash<int, QByteArray> PhotoModel::roleNames() const
{
    auto roleNames = QAbstractItemModel::roleNames();
    roleNames[ThumnailRole] = "thumbnail";
    roleNames[FilenameRole] = "filename";
    roleNames[EventIdRole] = "eventid";
    roleNames[TypeRole] = "type";
    return roleNames;
}

QString PhotoModel::mappedFile(const QString& file) const
{
    static auto map = QSettings().value("map").toMap();
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
    }
}