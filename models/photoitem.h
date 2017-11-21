#ifndef PHOTOITEM_H
#define PHOTOITEM_H

#include <QDateTime>
#include <QPixmap>
#include <QString>

class PhotoItem
{
public:
    PhotoItem(int photoid, const QDateTime& exposureTime, const QString& fileName);
    PhotoItem(int photoid, int eventId, const QDateTime& exposureTime, const QString& fileName, const QString& type);
    virtual ~PhotoItem();

    QString displayString() const;
    QString getThumbnailId() const;
    QString getFilename() const;
    QPixmap getIcon() const;
    int sortData() const;

    int getPhotoId() const;
    int getEventId() const;
    QDateTime getExposureTime() const;
    QString getType() const;

    friend bool operator<(const PhotoItem& lhs, const PhotoItem& rhs);

private:
    int photoId;
    int eventId;
    QDateTime exposureTime;
    QString fileName;
    QString type;
};

#endif  // PHOTOITEM_H
