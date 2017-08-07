#include "shotwin.h"

#include "eventitem.h"
#include "eventtreemodel.h"
#include "hidephotosproxymodel.h"
#include "monthitem.h"
#include "photoitem.h"
#include "yearitem.h"

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

    if (dynamic_cast<EventItem*>(item)) {
        handleEventSelected(index);
        emit photoListRequested();
    }
    else if (dynamic_cast<MonthItem*>(item) || dynamic_cast<YearItem*>(item)) {
        handleDirectorySelected(index);
        emit eventListRequested();
    }
}

void Shotwin::handleEventSelected(int index)
{
    auto clickedEvent = eventListModel->index(index, 0, QModelIndex());
    handleEventSelected(clickedEvent);
}

void Shotwin::handleEventSelected(const QModelIndex& index)
{
    if (index.isValid()) {
        auto eventItem = static_cast<EventItem*>(index.internalPointer());
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
        photoListModel->setTopLevelIndex(clickedEvent);
        auto treeClickedEvent = proxyModel->mapFromSource(eventListModel->mapToSource(clickedEvent));
        emit eventSelected(treeClickedEvent);
    }
}

void Shotwin::handlePhotoSelected(int index)
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
        emit photoInfoReady(info);
    }
}

void Shotwin::handleDirectorySelected(const QModelIndex& index)
{
    photoListModel->setTopLevelIndex(index);
    eventListModel->setTopLevelIndex(index);

    int photos = photoListModel->rowCount();
    auto firstIndex = photoListModel->index(0, 0);
    auto lastIndex = photoListModel->index(photos - 1, 0);
    auto firstPhoto = static_cast<PhotoItem*>(firstIndex.internalPointer());
    auto lastPhoto = static_cast<PhotoItem*>(lastIndex.internalPointer());

    DirectoryInfo info;
    info.from = firstPhoto->getExposureTime();
    info.to = lastPhoto->getExposureTime();
    info.items = photos;
    info.eventItems = eventListModel->rowCount();

    emit directoryInfoReady(info);
}
