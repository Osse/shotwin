#include "shotwin.h"

#include "datefilteredeventmodel.h"
#include "eventitem.h"
#include "eventmodel.h"
#include "eventortagfilteredphotomodel.h"
#include "photoitem.h"
#include "photomodel.h"
#include "tagmodel.h"
#include "treeproxymodel.h"

#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>

Shotwin::Shotwin(QObject* parent) : QObject(parent)
{
    map = QSettings().value("map").toMap();
}

bool Shotwin::initModels()
{
    if (!initDbViews())
        return false;

    photoModel = new PhotoModel(this);
    photoListModel = new EventOrTagFilteredPhotoModel(this);
    photoListModel->setSourceModel(photoModel);

    eventModel = new EventModel(this);
    eventListModel = new DateFilteredEventModel(this);
    eventListModel->setSourceModel(eventModel);

    eventTreeModel = new TreeProxyModel(this);
    eventTreeModel->setGroupingDataCb([](const QModelIndex& index) {
        auto eventItem = static_cast<const EventItem*>(index.internalPointer());
        auto date = eventItem->getStartTime();
        return std::vector<QVariant>{date.toString("yyyy"), date.toString("MMMM")};
    });
    eventTreeModel->setSourceDataCb([](const QModelIndex& index) {
        auto eventItem = static_cast<const EventItem*>(index.internalPointer());
        auto date = eventItem->getStartTime().date();
        return std::vector<QVariant>{date.year(), date.month()};
    });
    eventTreeModel->setSourceModel(eventModel);

    tagModel = new TagModel(this);
    return true;
}

QAbstractItemModel* Shotwin::getEventTree()
{
    return eventTreeModel;
}

QAbstractItemModel* Shotwin::getEventList()
{
    return eventListModel;
}

QAbstractItemModel* Shotwin::getPhotoList()
{
    return photoListModel;
}

QAbstractItemModel* Shotwin::getPhotoModel()
{
    return photoModel;
}

QAbstractItemModel* Shotwin::getTagModel()
{
    return tagModel;
}

void Shotwin::selectEvent(const QModelIndex& index)
{
    auto sourceIndex = eventTreeModel->mapToSource(index);
    if (sourceIndex.isValid()) {
        auto item = static_cast<EventItem*>(sourceIndex.internalPointer());
        photoListModel->setEventId(item->getEventId());
        emit photoListRequested();
    }
    else {
        if (index.parent().isValid()) {
            int year = eventTreeModel->data(index.parent(), TreeProxyModel::SourceDataRole).toInt();
            int month = eventTreeModel->data(index, TreeProxyModel::SourceDataRole).toInt();
            eventListModel->setFilterMonth(year, month);
        }
        else {
            int year = eventTreeModel->data(index, TreeProxyModel::SourceDataRole).toInt();
            eventListModel->setFilterYear(year);
        }
        emit eventListRequested();
    }
}

void Shotwin::openEvent(int index)
{
    auto clickedEvent = eventListModel->index(index, 0);
    auto sourceClickedEvent = eventListModel->mapToSource(clickedEvent);
    if (sourceClickedEvent.isValid()) {
        auto clickedEventInTree = eventTreeModel->mapFromSource(sourceClickedEvent);
        emit eventSelected(clickedEventInTree);

        auto item = static_cast<EventItem*>(sourceClickedEvent.internalPointer());
        photoListModel->setEventId(item->getEventId());
    }
}

void Shotwin::selectTag(const QModelIndex& index)
{
    auto tagItem = static_cast<TagModel::TagItem*>(index.internalPointer());
    photoListModel->setPhotoIds(tagItem->photos);
    emit photoListRequested();
}

bool Shotwin::initDbViews()
{
    QString createPhotoVideoView(
        "create temporary view PhotoVideoView as "
        "select id, event_id, exposure_time, filename, 'photo' as type from PhotoTable "
        "union all "
        "select id, event_id, exposure_time, filename, 'video' as type from VideoTable;");

    QString createEventViewWithStartTime(
        "create temporary view EventViewWithTimes as "
        "select e.id, e.name, pv.start_time, pv.end_time, pv.photos, pv.videos, e.primary_source_id from ( "
        "select event_id, min(exposure_time) as start_time, max(exposure_time) as end_time, "
        "count(case when type = 'photo' then 1 else null end) as photos, "
        "count(case when type = 'video' then 1 else null end) as videos "
        "from PhotoVideoView group "
        "by event_id) pv "
        "left outer join EventTable as e on pv.event_id = e.id;");

    QSqlQuery query(QSqlDatabase::database());

    if (!query.exec(createPhotoVideoView))
        return false;

    if (!query.exec(createEventViewWithStartTime))
        return false;

    return true;
}

QMap<QString, QVariant> Shotwin::getMap() const
{
    return map;
}

void Shotwin::setMap(const QMap<QString, QVariant>& value)
{
    map = value;
}

QString Shotwin::mappedFile(const QString& file)
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
