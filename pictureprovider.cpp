#include "pictureprovider.h"

#include <QDebug>
#include <QFile>
#include <QImageReader>
#include <QSettings>

PictureProvider::PictureProvider() : QQuickImageProvider(QQmlImageProviderBase::Image)
{
    map = QSettings().value("map").toMap();
}

PictureProvider::~PictureProvider()
{
}

QImage PictureProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    auto fileName = mappedFile(id);

    if (!QFile::exists(fileName))
        return QImage();

    QImageReader reader(fileName);
    reader.setAutoTransform(true);

    if (!reader.canRead())
        return QImage();

    auto image = reader.read();

    if (size)
        *size = image.size();

    return image;
}

void PictureProvider::setMap(const QMap<QString, QVariant>& value)
{
    map = value;
}

QString PictureProvider::mappedFile(const QString& file)
{
    QString fileName(file);
    for (auto it = map.begin(); it != map.end(); ++it) {
        if (fileName.startsWith(it.key())) {
            fileName.replace(it.key(), it.value().toString());
            return fileName;
        }
    }
    return QString();
}
