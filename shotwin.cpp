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

    photoListModel = new FilterFlattenProxyModel<PhotoItem>(this);
    photoListModel->setSourceModel(eventTreeModel);

    eventListModel = new FilterFlattenProxyModel<EventItem>(this);
    eventListModel->setSourceModel(eventTreeModel);

    photoModel = new PhotoModel(this);
    photoModel4 = new EventFilteredPhotoModel(this);
    photoModel4->setSourceModel(photoModel);
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

void Shotwin::handleTreeClicked(const QModelIndex& index)
{
    photoListModel->setTopLevelIndex(index);
    eventListModel->setTopLevelIndex(index);

    auto item = static_cast<EventTreeItem*>(index.internalPointer());

    auto eventItem = dynamic_cast<EventItem*>(item);
    if (eventItem) {
        photoModel4->setEventId(eventItem->getEventId());
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

        emit kek2(info);
    }
}

void Shotwin::handleEventViewClicked(int index)
{
    auto clickedEvent = eventListModel->index(index, 0, QModelIndex());
    if (clickedEvent.isValid()) {
        photoListModel->setTopLevelIndex(clickedEvent);
        auto treeClickedEvent = proxyModel->mapFromSource(eventListModel->mapToSource(clickedEvent));
        emit eventSelected(treeClickedEvent);
    }
}

void Shotwin::handlePhotoViewClicked(int index)
{
    auto clickedPhoto = photoListModel->index(index, 0, QModelIndex());
    if (clickedPhoto.isValid()) {
        auto photoItem = static_cast<PhotoItem*>(clickedPhoto.internalPointer());
        photoItem->populateFromExif();
        PhotoInfo info;
        info.exposureTime = photoItem->getExposureTime();
        info.exposureString = photoItem->getExposureString();
        info.size = photoItem->getSize();
        info.title = photoItem->getFilename().split("/").last();
        emit kek(info);
    }
}

QAbstractItemModel* Shotwin::getPhotoModel()
{
    return photoModel4;
}
