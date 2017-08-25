#include "framegrabber.h"

FrameGrabber::FrameGrabber(QObject* parent) : QObject(parent)
{
    mediaPlayer.setVideoOutput(&videoSurface);
    connect(&mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &FrameGrabber::debugMedia);
    connect(&mediaPlayer,
            static_cast<void (QMediaPlayer::*)(QMediaPlayer::Error)>(&QMediaPlayer::error),
            this,
            &FrameGrabber::debugError);
    connect(&videoSurface, &VideoSurface::stopPlease, this, [&]() {
        qDebug() << "stopPlease";
        mediaPlayer.pause();
    });
}

QImage FrameGrabber::grab(const QString& filename)
{
    mediaPlayer.setMedia(QUrl::fromLocalFile(filename));
    videoSurface.setFilename(filename);
    return QImage();
}

void FrameGrabber::debugMedia(QMediaPlayer::MediaStatus status)
{
    qDebug() << status;
    if (status == QMediaPlayer::LoadedMedia) {
        mediaPlayer.play();
    }
}

void FrameGrabber::debugError(QMediaPlayer::Error error)
{
    qDebug() << error;
}

VideoSurface::VideoSurface(QObject* parent) : QAbstractVideoSurface(parent)
{
}

bool VideoSurface::present(const QVideoFrame& frame)
{
    if (!done) {
        qDebug() << __PRETTY_FUNCTION__;
        qDebug() << "frame mapped:" << frame.isMapped();
        QVideoFrame copy(frame);
        qDebug() << frame.availableMetaData();
        if (copy.map(QAbstractVideoBuffer::ReadOnly)) {
            qDebug() << "copy mapped:" << copy.isMapped();
            auto size = copy.size();
            QImage i(
                copy.bits(), copy.width(), copy.height(), QVideoFrame::imageFormatFromPixelFormat(copy.pixelFormat()));
            qDebug() << "filename: " << filename + ".jpg";
            if (!i.isNull())
                qDebug() << "save succeeded:" << i.save(filename + QString::number(n++) + ".jpg");
            else
                qDebug() << "image is null";
            done = true;
        }
    }
    emit stopPlease();
    stop();
    return true;
}

QList<QVideoFrame::PixelFormat> VideoSurface::supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const
{
    QList<QVideoFrame::PixelFormat> list;
    for (int i = 0; i < 33; i++)
        list.append((QVideoFrame::PixelFormat)i);
    return list;
}

// bool VideoSurface::start(const QVideoSurfaceFormat& format)
//{
//    qDebug() << __PRETTY_FUNCTION__;
//    return QAbstractVideoSurface::start(format);
//}

// void VideoSurface::stop()
//{
//    qDebug() << __PRETTY_FUNCTION__;
//    QAbstractVideoSurface::stop();
//}

bool VideoSurface::isFormatSupported(const QVideoSurfaceFormat& format) const
{
    qDebug() << __PRETTY_FUNCTION__;
    return true;
}

QString VideoSurface::getFilename() const
{
    return filename;
}

void VideoSurface::setFilename(const QString& value)
{
    filename = value;
}
