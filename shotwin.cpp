#include "shotwin.h"

#include "datefilteredeventmodel.h"
#include "eventitem.h"
#include "eventmodel.h"
#include "eventortagfilteredphotomodel.h"
#include "photoitem.h"
#include "photomodel.h"
#include "tagmodel.h"
#include "treeproxymodel.h"

#include <QAbstractItemView>
#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStringList>

Shotwin::Shotwin(QObject* parent) : QObject(parent)
{
}

bool Shotwin::initModels()
{
    if (!initDbViews())
        return false;

    photoModel = new PhotoModel(map, this);
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

    eventTreeModel->setComp([](auto v1, auto v2) { return v1 > v2; });
    eventTreeModel->setSourceDataCb([](const QModelIndex& index) {
        auto eventItem = static_cast<const EventItem*>(index.internalPointer());
        auto date = eventItem->getStartTime().date();
        return std::vector<QVariant>{date.year(), date.month()};
    });
    eventTreeModel->setSourceModel(eventModel);
    eventTreeModel->dump("original.txt");

    fileSystemModel = new TreeProxyModel(this);
    fileSystemModel->setGroupingDataCb([](const QModelIndex& index) {
        auto photoItem = static_cast<const PhotoItem*>(index.internalPointer());
        auto path = photoItem->getFilename();
        auto split = path.split("/", QString::SkipEmptyParts);
        return std::vector<QVariant>(split.begin(), split.begin() + split.length() - 1);
    });
    fileSystemModel->setAlternateDisplayDataCb(
        [](const QVariant& value) { return value.toString().split("/", QString::SkipEmptyParts).last(); });
    fileSystemModel->setSourceModel(photoModel);

    tagModel = new TagModel(this);
    return true;
}

QAbstractItemModel* Shotwin::getEventTree() const
{
    return eventTreeModel;
}

QAbstractItemModel* Shotwin::getEventList() const
{
    return eventListModel;
}

QAbstractItemModel* Shotwin::getPhotoList() const
{
    return photoListModel;
}

QAbstractItemModel* Shotwin::getPhotoModel() const
{
    return photoModel;
}

QAbstractItemModel* Shotwin::getTagModel() const
{
    return tagModel;
}

QAbstractItemModel* Shotwin::getFileSystemModel() const
{
    return fileSystemModel;
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
    emit tagSelected(QModelIndex());
}

void Shotwin::openEvent(int index)
{
    auto clickedEvent = eventListModel->index(index, 0);
    auto sourceClickedEvent = eventListModel->mapToSource(clickedEvent);
    if (sourceClickedEvent.isValid()) {
        auto clickedEventInTree = eventTreeModel->mapFromSource(sourceClickedEvent);
        emit eventSelected(clickedEventInTree);
        emit tagSelected(QModelIndex());

        auto item = static_cast<EventItem*>(sourceClickedEvent.internalPointer());
        photoListModel->setEventId(item->getEventId());
    }
}

void Shotwin::selectTag(const QModelIndex& index)
{
    auto tagItem = static_cast<TagModel::TagItem*>(index.internalPointer());
    photoListModel->setPhotoIds(tagItem->photos);
    emit photoListRequested();
    emit eventSelected(QModelIndex());
}

void Shotwin::selectTagByString(const QString& tag)
{
    auto tagIndex = tagModel->match(tagModel->index(0, 0), Qt::DisplayRole, tag);
    selectTag(tagIndex.first());
    emit tagSelected(tagIndex.first());
    emit eventSelected(QModelIndex());
}

void Shotwin::resetFilterAndShowTree()
{
    auto view = static_cast<QAbstractItemView*>(sender());
    if (view == nullptr)
        return;

    auto model = view->model();

    if (model == eventTreeModel) {
        eventListModel->resetFilter();
        emit eventListRequested();
    }
    else if (model == fileSystemModel) {
        photoListModel->setEventIds({});
        emit photoListRequested();
    }
    else if (model == tagModel) {
        photoListModel->setPhotoIds(tagModel->getAllTaggedPhotos());
        emit photoListRequested();
    }
}

bool Shotwin::initDbViews()
{
    QString createPhotoVideoView(
        "create temporary view PhotoVideoView as "
        "select id, event_id, exposure_time, filename, rating, 'photo' as type from PhotoTable "
        "union all "
        "select id, event_id, exposure_time, filename, rating, 'video' as type from VideoTable;");

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
    if (photoModel)
        photoModel->setMap(map);
}

QStringList Shotwin::getTagsForPhoto(int id) const
{
    return tagModel->getTagsForPhoto(id);
}
