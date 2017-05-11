#ifndef PHOTOLISTMODEL_H
#define PHOTOLISTMODEL_H

#include <QAbstractListModel>

class EventItem;

class PhotoListModel : public QAbstractListModel
{
public:
    PhotoListModel(QObject* parent = nullptr);
    ~PhotoListModel();

    virtual int rowCount(const QModelIndex& parent) const;
    virtual QVariant data(const QModelIndex& index, int role) const;

    EventItem* getEvent() const;
    void setEventFromIndex(const QModelIndex& index);
    void setEvent(EventItem* value);

private:
    EventItem* event = nullptr;
};

#endif  // PHOTOLISTMODEL_H
