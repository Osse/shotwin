#include "shotwin.h"

#include "eventitem.h"
#include "eventtreemodel.h"
#include "hidephotosproxymodel.h"
#include "photoitem.h"

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

    if (dynamic_cast<EventItem*>(item))
        emit photoListRequested();
    else
        emit eventListRequested();
}
