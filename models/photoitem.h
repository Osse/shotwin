#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include "eventtreeitem.h"

#include <QDateTime>
#include <QSize>
#include <QString>

class PhotoItem : public EventTreeItem
{
public:
    PhotoItem(EventTreeItem* parent,
              int photoid,
              const QDateTime& exposureTime,
              const QString& fileName,
              const QString& mappedFileName);
    virtual ~PhotoItem();

    QString displayString() const override;
    QString getThumbnailId() const override;
    QString getFilename() const override;
    QPixmap getIcon() const override;
    int sortData() const override;

    QDateTime getExposureTime() const;
    QDateTime getMappedFileName() const;
    QString getExposureString() const;
    QSize getSize() const;

    void populateFromExif();

    friend bool operator<(const PhotoItem& lhs, const PhotoItem& rhs);

private:
    int photoId;
    QDateTime exposureTime;
    QString fileName;
    QString mappedFileName;
    QSize size;
    QString exposureString{"%1/%2 s, f/%3, ISO %4"};
    bool populatedDone = false;
};

#endif  // PHOTOITEM_H
