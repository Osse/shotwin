#include "thumbnailprovider.h"

#include "photomodel.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#include <algorithm>
#include <memory>

ThumbnailProvider::ThumbnailProvider(QAbstractItemModel* photoListModel)
    : QQuickImageProvider(QQmlImageProviderBase::Image), photoListModel(dynamic_cast<PhotoModel*>(photoListModel))
{
    auto cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
    if (!cachePaths.empty()) {
        cachePath = cachePaths.first();
        QDir cacheDir(cachePath);
        cacheDir.mkpath(cachePath + "/thumbs360");
        cacheDir.mkpath(cachePath + "/thumbs128");
        cacheDir.mkpath(cachePath + "/videoframes");
    }

    ffmpegCmd = QSettings().value(QStringLiteral("ffmpegpath")).toString();
}

QImage ThumbnailProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize)
{
    if (requestedSize == QSize(-1, -1))
        return QImage();

    int thumbSize = std::max(requestedSize.width(), requestedSize.height()) > 128 ? 360 : 128;

    QString thumbPath = QStringLiteral("%1/thumbs%2/%3.jpg").arg(cachePath, QString::number(thumbSize), id);
    QString videoFramePath = QStringLiteral("%1/videoframes/%2.jpg").arg(cachePath, id);

    if (QFile::exists(thumbPath))
        return QImage(thumbPath);

    QString filename;
    QString type;

    auto matches = photoListModel->match(photoListModel->index(0, 0), PhotoModel::ThumnailRole, id);
    QModelIndex match;
    if (!matches.empty())
        match = matches.first();
    else
        return generateFallback(requestedSize);

    filename = photoListModel->data(match, PhotoModel::MappedFilenameRole).toString();
    type = photoListModel->data(match, PhotoModel::TypeRole).toString();

    QImage image;
    if (type == QLatin1String("photo"))
        image = generateThumbnail(thumbSize, thumbPath, filename);
    else {
        if (!QFile::exists(videoFramePath))
            grabVideoFrame(videoFramePath, filename);
        image = generateThumbnail(thumbSize, thumbPath, videoFramePath);
    }

    if (image.isNull())
        image = generateFallback(requestedSize);

    if (size)
        *size = image.size();

    return image;
}

QImage ThumbnailProvider::generateFallback(QSize size)
{
    QImage image(size, QImage::Format_RGB32);
    image.fill("steelblue");
    return image;
}

QImage ThumbnailProvider::generateThumbnail(int thumbSize, const QString& thumbPath, const QString& filename)
{
    QImageReader reader(filename);
    reader.setAutoTransform(true);

    if (!reader.canRead())
        return QImage();

    auto image = reader.read();

    auto smallRough = image.scaled(thumbSize * 2, thumbSize * 2, Qt::KeepAspectRatio, Qt::FastTransformation);
    auto thumbnail = smallRough.scaled(thumbSize, thumbSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    thumbnail.save(thumbPath);

    return thumbnail;
}

QImage ThumbnailProvider::grabVideoFrame(const QString& thumbPath, const QString& filename)
{
    auto ffmpeg = std::make_unique<QProcess>();

    QStringList arguments = {
        "-loglevel", "panic", "-i", filename, "-ss", "0.5", "-q:v", "2", "-vframes", "1", thumbPath};

    ffmpeg->start(ffmpegCmd, arguments);
    ffmpeg->waitForFinished(20000);

    if (!ffmpeg->exitCode()) {
        return QImage(thumbPath);
    }
    else
        return QImage();
}

void ThumbnailProvider::setFfmpegCmd(const QString& value)
{
    ffmpegCmd = value;
}
