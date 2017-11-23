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
    QAbstractItemModel* getFileSystemModel();

    QMap<QString, QVariant> getMap() const;
    void setMap(const QMap<QString, QVariant>& value);

    Q_INVOKABLE QString mappedFile(const QString& file);
    Q_INVOKABLE QStringList getTagsForPhoto(int id);

signals:
    void eventListRequested();
    void photoListRequested();
    void eventSelected(const QModelIndex& index);
    void tagSelected(const QModelIndex& index);

public slots:
    void selectEvent(const QModelIndex& index);
    void openEvent(int index);
    void selectTag(const QModelIndex& index);
    void selectTagByString(const QString& tag);

private:
    bool initDbViews();

private:
    EventModel* eventModel = nullptr;
    PhotoModel* photoModel = nullptr;

    EventOrTagFilteredPhotoModel* photoListModel = nullptr;
    DateFilteredEventModel* eventListModel = nullptr;

    TreeProxyModel* eventTreeModel = nullptr;
    TreeProxyModel* fileSystemModel = nullptr;

    TagModel* tagModel = nullptr;

    QMap<QString, QVariant> map;
};

#endif  // SHOTWIN_H
