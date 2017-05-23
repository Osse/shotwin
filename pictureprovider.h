#ifndef PICTUREPROVIDER_H
#define PICTUREPROVIDER_H

#include <QMap>
#include <QQuickImageProvider>
#include <QString>

class PictureProvider : public QQuickImageProvider
{
public:
    PictureProvider();
    ~PictureProvider();

    QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize);
    void setMap(const QMap<QString, QVariant>& value);

private:
    QString mappedFile(const QString& file);

private:
    QMap<QString, QVariant> map;
};

#endif  // PICTUREPROVIDER_H
