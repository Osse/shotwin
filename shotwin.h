#ifndef SHOTWIN_H
#define SHOTWIN_H

#include "filterflattenproxymodel.h"

#include <QObject>

class QAbstractItemModel;
class EventTreeModel;
class HidePhotosProxyModel;

class Shotwin : public QObject
{
    Q_OBJECT
public:
    explicit Shotwin(QObject* parent = nullptr);

    void initModels();

    QAbstractItemModel* getEventTree();
    QAbstractItemModel* getEventList();
    QAbstractItemModel* getPhotoList();

signals:
    void eventListRequested();
    void photoListRequested();
    void eventSelected(const QModelIndex& index);

public slots:
    void selectEvent(const QModelIndex& index);
    void openEvent(int index);

private:
    EventTreeModel* eventTreeModel = nullptr;
    HidePhotosProxyModel* proxyModel = nullptr;

    FilterFlattenProxyModel<PhotoItem>* photoListModel = nullptr;
    FilterFlattenProxyModel<EventItem>* eventListModel = nullptr;
};

#endif  // SHOTWIN_H
