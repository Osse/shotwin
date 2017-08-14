#include "shotwin.h"

#include "eventfilteredphotomodel.h"
#include "eventitem.h"
#include "eventtreemodel.h"
#include "hidephotosproxymodel.h"
#include "photoitem.h"
#include "photomodel.h"

Shotwin::Shotwin(QObject* parent) : QObject(parent)
{
}

void Shotwin::initModels()
{
    eventTreeModel = new EventTreeModel(this);
    proxyModel = new HidePhotosProxyModel(this);
    proxyModel->setSourceModel(eventTreeModel);

    photoModel = new PhotoModel(this);
    photoListModel = new EventFilteredPhotoModel(this);
    photoListModel->setSourceModel(photoModel);

    eventListModel = new FilterFlattenProxyModel<EventItem>(this);
    eventListModel->setSourceModel(eventTreeModel);
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

void Shotwin::selectEvent(const QModelIndex& index)
{
    eventListModel->setTopLevelIndex(index);

    auto item = static_cast<EventTreeItem*>(index.internalPointer());

    auto eventItem = dynamic_cast<EventItem*>(item);
    if (eventItem) {
        photoListModel->setEventId(eventItem->getEventId());
        emit photoListRequested();
    }
    else
        emit eventListRequested();
}

void Shotwin::handleEventSelected(int index)
{
    auto clickedEvent = eventListModel->index(index, 0, QModelIndex());
    if (clickedEvent.isValid()) {
        auto eventItem = static_cast<EventItem*>(clickedEvent.internalPointer());
        EventInfo info;
        info.title = eventItem->displayString();
        info.from = eventItem->getStartTime();
        info.to = eventItem->getEndTime();
        info.items = eventItem->childCount();

        emit eventInfoReady(info);
    }
}

void Shotwin::openEvent(int index)
{
    auto clickedEvent = eventListModel->index(index, 0, QModelIndex());
    if (clickedEvent.isValid()) {
        auto treeClickedEvent = proxyModel->mapFromSource(eventListModel->mapToSource(clickedEvent));
        emit eventSelected(treeClickedEvent);
    }
}

void Shotwin::handlePhotoSelected(int index)
{
    auto clickedPhoto = photoListModel->index(index, 0, QModelIndex());
    if (clickedPhoto.isValid()) {
        auto sourceIndex = photoListModel->mapToSource(clickedPhoto);
        auto photoItem = static_cast<PhotoItem*>(sourceIndex.internalPointer());
        photoItem->populateFromExif();
        PhotoInfo info;
        info.exposureTime = photoItem->getExposureTime();
        info.exposureString = photoItem->getExposureString();
        info.size = photoItem->getSize();
        info.title = photoItem->getFilename().split("/").last();
        emit photoInfoReady(info);
    }
}
