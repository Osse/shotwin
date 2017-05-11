#include "eventtreemodel.h"

#include "eventitem.h"
#include "monthitem.h"
#include "photoitem.h"
#include "rootitem.h"
#include "yearitem.h"

#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>

const QString eventTreeQuery(
    "select * from (select p.id as photoid, p.exposure_time as exposure_time, p.filename as "
    "filename, e.id as event_id, e.name as event_name from PhotoTable as p "
    "left outer join EventTable as e on p.event_id = e.id) where event_id is not null order by "
    "event_id asc, exposure_time asc");

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

    auto eventItem = dynamic_cast<EventItem*>(parentItem);
    if (eventItem)
        return 0;

    return parentItem->childCount();
}

QVariant EventTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    auto item = static_cast<EventTreeItem*>(index.internalPointer());
    return item->displayString();
}

QVariant EventTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section != 0 || orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    return QVariant("Events");
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
        int photoId = query.value("photoid").toInt();
        auto exposureTime = QDateTime::fromSecsSinceEpoch(query.value("exposure_time").toInt());
        QString fileName = query.value("filename").toString();
        int eventId = query.value("event_id").toInt();
        QString eventName = query.value("event_name").toString();

        auto date = exposureTime.date();

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
            auto newEvent = new EventItem(months[{year, month}], eventId, eventName, exposureTime);
            events[eventId] = newEvent;
            months[{year, month}]->appendChild(newEvent);
        }

        auto photo = new PhotoItem(events[eventId], photoId, exposureTime, fileName);
        events[eventId]->appendChild(photo);
    }
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
