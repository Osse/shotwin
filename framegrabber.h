#ifndef FRAMEGRABBER_H
#define FRAMEGRABBER_H

#include <QAbstractVideoSurface>
#include <QMediaPlayer>

class VideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    VideoSurface(QObject* parent = nullptr);

    bool present(const QVideoFrame &frame);
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(QAbstractVideoBuffer::HandleType handleType) const;

//    bool start(const QVideoSurfaceFormat &format) override;
//    void stop() override;

    bool isFormatSupported(const QVideoSurfaceFormat &format) const;
    QString getFilename() const;
    void setFilename(const QString& value);
signals:
    void stopPlease();

private:
    QString filename;
    int n = 0;
    int done = false;
};

class FrameGrabber : public QObject
{
public:
    FrameGrabber(QObject* parent = nullptr);

    QImage grab(const QString& filename);
private:
    QMediaPlayer mediaPlayer;
    VideoSurface videoSurface;
    QString filename;

    void debugMedia(QMediaPlayer::MediaStatus status);
    void debugError(QMediaPlayer::Error error);
};

#endif // FRAMEGRABBER_H
