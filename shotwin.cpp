#include "shotwin.h"

#include "eventitem.h"
#include "eventortagfilteredphotomodel.h"
#include "eventtreemodel.h"
#include "hidephotosproxymodel.h"
#include "photoitem.h"
#include "photomodel.h"
#include "thumbnailcreator.h"

#include <QSettings>
#include <QSqlDatabase>
#include <QSqlQuery>

Shotwin::Shotwin(QObject* parent) : QObject(parent)
{
    map = QSettings().value("map").toMap();
}

Shotwin::~Shotwin()
{
}

bool Shotwin::initModels()
{
    if (!initDbViews())
        return false;

    eventTreeModel = new EventTreeModel(this);
    proxyModel = new HidePhotosProxyModel(this);
    proxyModel->setSourceModel(eventTreeModel);

    photoModel = new PhotoModel(this);
    photoListModel = new EventOrTagFilteredPhotoModel(this);
    photoListModel->setSourceModel(photoModel);

    eventListModel = new FilterFlattenProxyModel<EventItem>(this);
    eventListModel->setSourceModel(eventTreeModel);

    // startThumbnailing();

    return true;
}

QAbstractItemModel* Shotwin::getEventTree()
{
    return proxyModel;
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

void Shotwin::selectEvent(const QModelIndex& index)
{
    eventListModel->setTopLevelIndex(index);

    auto item = static_cast<EventTreeItem*>(index.internalPointer());

    auto eventItem = dynamic_cast<EventItem*>(item);
    if (dynamic_cast<EventItem*>(item)) {
        photoListModel->setEventId(eventItem->getEventId());
        emit photoListRequested();
    }
    else
        emit eventListRequested();
}

void Shotwin::openEvent(int index)
{
    auto clickedEvent = eventListModel->index(index, 0, QModelIndex());
    if (clickedEvent.isValid()) {
        auto treeClickedEvent = proxyModel->mapFromSource(eventListModel->mapToSource(clickedEvent));
        emit eventSelected(treeClickedEvent);
    }
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

void Shotwin::startThumbnailing()
{
    auto kek = new ThumbnailCreator(this);
    QMap<QString, QString> files;

    auto pm = getPhotoList();
    for (int i = 0; i < pm->rowCount(); i++) {
        auto index = pm->index(i, 0);
        files[pm->data(index, PhotoModel::ThumnailRole).toString()] =
            mappedFile(pm->data(index, PhotoModel::FilenameRole).toString());
    }

    kek->start(files);
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
