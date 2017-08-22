#include "eventtreemodel.h"

#include "eventitem.h"
#include "monthitem.h"
#include "photoitem.h"
#include "rootitem.h"
#include "yearitem.h"

#include <QDateTime>
#include <QDebug>
#include <QFont>
#include <QSqlDatabase>
#include <QSqlQuery>

const QString eventTreeQuery("select * from EventViewWithTimes");

EventTreeModel::EventTreeModel(QObject* parent) : QAbstractItemModel(parent)
{
    init();
}

EventTreeModel::~EventTreeModel()
{
}

QModelIndex EventTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    EventTreeItem* parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = getItem(parent);

    auto childItem = parentItem->getChild(row);

    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex EventTreeModel::parent(const QModelIndex& child) const
{
    if (!child.isValid())
        return QModelIndex();

    auto childItem = getItem(child);
    auto parentItem = childItem->getParent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->getParent()->getRow(parentItem), 0, parentItem);
}

int EventTreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0)
        return 0;

    EventTreeItem* parentItem;
    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = getItem(parent);

    return parentItem->childCount();
}

QVariant EventTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = static_cast<EventTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole)
        return item->displayString();
    else if (role == Qt::DecorationRole)
        return item->getIcon();
    else if (role == ThumnailRole)
        return item->getThumbnailId();
    else if (role == FilenameRole)
        return item->getFilename();
    else if (role == TimespanRole)
        return item->getEventTimeSpan();
    else if (role == PhotoCountRole) {
        auto eventItem = dynamic_cast<EventItem*>(item);
        if (eventItem)
            return eventItem->getPhotos();
    }
    else if (role == VideoCountRole) {
        auto eventItem = dynamic_cast<EventItem*>(item);
        if (eventItem)
            return eventItem->getVideos();
    }
    else
        return QVariant();
}

QVariant EventTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QHash<int, QByteArray> EventTreeModel::roleNames() const
{
    auto roleNames = QAbstractItemModel::roleNames();
    roleNames[ThumnailRole] = "thumbnail";
    roleNames[FilenameRole] = "filename";
    roleNames[TimespanRole] = "timespan";
    roleNames[PhotoCountRole] = "photocount";
    roleNames[VideoCountRole] = "videocount";
    return roleNames;
}

void EventTreeModel::init()
{
    auto db = QSqlDatabase::database();

    QSqlQuery query(eventTreeQuery, db);

    if (!query.exec())
        return;

    rootItem = new RootItem;

    QMap<int, YearItem*> years;
    QMap<QPair<int, int>, MonthItem*> months;
    QMap<int, EventItem*> events;

    while (query.next()) {
        auto startTime = QDateTime::fromSecsSinceEpoch(query.value("start_time").toInt());
        auto endTime = QDateTime::fromSecsSinceEpoch(query.value("end_time").toInt());
        int eventId = query.value("id").toInt();
        QString eventName = query.value("name").toString();
        QString primarySourceId = query.value("primary_source_id").toString();
        int photos = query.value("photos").toInt();
        int videos = query.value("videos").toInt();

        auto date = startTime.date();

        int year = date.year();
        int month = date.month();

        if (!years.contains(year)) {
            auto newYear = new YearItem(rootItem, year);
            years[year] = newYear;
            rootItem->appendChild(newYear);
        }

        if (!months.contains({year, month})) {
            auto newMonth = new MonthItem(years[year], month);
            months[{year, month}] = newMonth;
            years[year]->appendChild(newMonth);
        }

        if (!events.contains(eventId)) {
            auto newEvent = new EventItem(
                months[{year, month}], eventId, eventName, startTime, endTime, photos, videos, primarySourceId);
            events[eventId] = newEvent;
            months[{year, month}]->appendChild(newEvent);
        }
    }

    sort();
}

void EventTreeModel::sort()
{
    rootItem->sortChildren();
}

EventTreeItem* EventTreeModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        auto* item = static_cast<EventTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

int EventTreeModel::columnCount(const QModelIndex& parent) const
{
    return 1;
}
