#include "eventmodel.h"

#include <QDateTime>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlQuery>

const QString eventListQuery("select * from EventViewWithTimes");

EventModel::EventModel(QObject* parent) : QAbstractListModel(parent)
{
    init();
}

EventModel::~EventModel()
{
}

int EventModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;

    return eventList.size();
}

QVariant EventModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (row >= eventList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return eventList[row].displayString();
    else if (role == Qt::DecorationRole)
        return eventList[row].getIcon();
    else if (role == StartTimeRole)
        return eventList[row].getStartTime();
    else if (role == EndTimeRole)
        return eventList[row].getEndTime();
    else if (role == ThumnailRole)
        return eventList[row].getThumbnailId();
    else if (role == TimespanRole)
        return eventList[row].getEventTimeSpan();
    else if (role == PhotoCountRole)
        return eventList[row].getPhotos();
    else if (role == VideoCountRole)
        return eventList[row].getVideos();
    else
        return QVariant();
}

QModelIndex EventModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() || column > 0)
        return QModelIndex();

    return createIndex(row, column, const_cast<EventItem*>(&eventList[row]));
}

QVariant EventModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0 || orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::DisplayRole)
        return QVariant("Events");

    if (role == Qt::FontRole) {
        auto font = QAbstractItemModel::headerData(section, orientation, Qt::FontRole).value<QFont>();
        font.setBold(true);
        return font;
    }

    return QVariant();
}

QHash<int, QByteArray> EventModel::roleNames() const
{
    auto roleNames = QAbstractItemModel::roleNames();
    roleNames[ThumnailRole] = "thumbnail";
    roleNames[StartTimeRole] = "starttime";
    roleNames[EndTimeRole] = "endtime";
    roleNames[TimespanRole] = "timespan";
    roleNames[PhotoCountRole] = "photocount";
    roleNames[VideoCountRole] = "videocount";
    return roleNames;
}

void EventModel::sort(int column, Qt::SortOrder order)
{
    if (column != 0)
        return;

    emit layoutAboutToBeChanged();
    if (order == Qt::AscendingOrder)
        std::sort(eventList.begin(), eventList.end(), [](const auto& e1, const auto& e2) {
            return e1.getStartTime() < e2.getStartTime();
        });
    else
        std::sort(eventList.begin(), eventList.end(), [](const auto& e1, const auto& e2) {
            return e1.getStartTime() > e2.getStartTime();
        });
    emit layoutChanged();
}

void EventModel::init()
{
    auto db = QSqlDatabase::database();

    QSqlQuery query(eventListQuery, db);

    if (!query.exec())
        return;

    while (query.next()) {
        auto startTime = QDateTime::fromSecsSinceEpoch(query.value("start_time").toInt());
        auto endTime = QDateTime::fromSecsSinceEpoch(query.value("end_time").toInt());
        int eventId = query.value("id").toInt();
        QString eventName = query.value("name").toString();
        QString primarySourceId = query.value("primary_source_id").toString();
        int photos = query.value("photos").toInt();
        int videos = query.value("videos").toInt();

        eventList.push_back(EventItem(eventId, eventName, startTime, endTime, photos, videos, primarySourceId));
    }

    sort(0, Qt::DescendingOrder);
}
