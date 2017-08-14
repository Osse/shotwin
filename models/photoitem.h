#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include "eventtreeitem.h"

#include <QDateTime>
#include <QString>

class PhotoItem : public EventTreeItem
{
public:
    PhotoItem(EventTreeItem* parent, int photoid, const QDateTime& exposureTime, const QString& fileName);
    virtual ~PhotoItem();

    QString displayString() const override;
    QString getThumbnailId() const override;
    QString getFilename() const override;
    QPixmap getIcon() const override;
    int sortData() const override;

    QDateTime getExposureTime() const;

    friend bool operator<(const PhotoItem& lhs, const PhotoItem& rhs);

private:
    int photoId;
    QDateTime exposureTime;
    QString fileName;
};

#endif  // PHOTOITEM_H
