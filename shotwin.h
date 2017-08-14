#ifndef SHOTWIN_H
#define SHOTWIN_H

#include "filterflattenproxymodel.h"
#include "infobox.h"

#include <QObject>

class QAbstractItemModel;
class EventTreeModel;
class HidePhotosProxyModel;
class PhotoModel;
class EventFilteredPhotoModel;

class Shotwin : public QObject
{
    Q_OBJECT
public:
    explicit Shotwin(QObject* parent = nullptr);

    void initModels();

    QAbstractItemModel* getEventTree();
    QAbstractItemModel* getEventList();
    QAbstractItemModel* getPhotoList();
    QAbstractItemModel* getPhotoModel();

signals:
    void eventListRequested();
    void photoListRequested();
    void eventSelected(const QModelIndex& index);
    void kek(const PhotoInfo& info);
    void kek2(const EventInfo& info);

public slots:
    void handleTreeClicked(const QModelIndex& index);
    void handleEventSelected(int index);
    void handleEventViewClicked(int index);
    void handlePhotoViewClicked(int index);

private:
    EventTreeModel* eventTreeModel = nullptr;
    HidePhotosProxyModel* proxyModel = nullptr;

    PhotoModel* photoModel = nullptr;

    EventFilteredPhotoModel* photoModel4 = nullptr;

    FilterFlattenProxyModel<PhotoItem>* photoListModel = nullptr;
    FilterFlattenProxyModel<EventItem>* eventListModel = nullptr;
};

#endif  // SHOTWIN_H
