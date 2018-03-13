#ifndef SHOTWIN_H
#define SHOTWIN_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QVariant>

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

    QAbstractItemModel* getEventTree() const;
    QAbstractItemModel* getEventList() const;
    QAbstractItemModel* getPhotoList() const;
    QAbstractItemModel* getPhotoModel() const;
    QAbstractItemModel* getTagModel() const;
    QAbstractItemModel* getFileSystemModel() const;

    QMap<QString, QVariant> getMap() const;
    void setMap(const QMap<QString, QVariant>& value);

    Q_INVOKABLE QString mappedFile(const QString& file) const;
    Q_INVOKABLE QStringList getTagsForPhoto(int id) const;

signals:
    void eventListRequested();
    void photoListRequested();
    void eventSelected(const QModelIndex& index);
    void tagSelected(const QModelIndex& index);

public slots:
    void selectEventFromQML(const QVariant& index);
    void selectEvent(const QModelIndex& index);
    void openEvent(int index);
    void selectTag(const QModelIndex& index);
    void selectTagByString(const QString& tag);
    void resetFilterAndShowTree();

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
