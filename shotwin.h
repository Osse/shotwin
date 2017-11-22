#ifndef SHOTWIN_H
#define SHOTWIN_H

#include "filterflattenproxymodel.h"

#include <QObject>

class QAbstractItemModel;
class DateFilteredEventModel;
class EventModel;
class EventOrTagFilteredPhotoModel;
class PhotoModel;
class TreeProxyModel;
class TagModel;

class Shotwin : public QObject
{
    Q_OBJECT
public:
    explicit Shotwin(QObject* parent = nullptr);

    bool initModels();

    QAbstractItemModel* getEventTree();
    QAbstractItemModel* getEventList();
    QAbstractItemModel* getPhotoList();
    QAbstractItemModel* getPhotoModel();
    QAbstractItemModel* getTagModel();

    QMap<QString, QVariant> getMap() const;
    void setMap(const QMap<QString, QVariant>& value);

    Q_INVOKABLE QString mappedFile(const QString& file);

signals:
    void eventListRequested();
    void photoListRequested();
    void eventSelected(const QModelIndex& index);

public slots:
    void selectEvent(const QModelIndex& index);
    void openEvent(int index);
    void selectTag(const QModelIndex& index);

private:
    bool initDbViews();

private:
    EventModel* eventModel = nullptr;
    PhotoModel* photoModel = nullptr;

    EventOrTagFilteredPhotoModel* photoListModel = nullptr;
    DateFilteredEventModel* eventListModel = nullptr;

    TreeProxyModel* eventTreeModel = nullptr;

    TagModel* tagModel = nullptr;

    QMap<QString, QVariant> map;
};

#endif  // SHOTWIN_H
