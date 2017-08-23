#ifndef THUMBNAILCREATOR_H
#define THUMBNAILCREATOR_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QThread>

class Worker : public QObject
{
    Q_OBJECT

public:
    Worker(QObject* parent = nullptr);
    virtual ~Worker();
    void generateThumbnails();

    void setFiles(const QMap<QString, QString>& value);
    void requestStop();

signals:
    void allDone();
    void oneDone(const QString& thumbnail);

private:
    bool generateOneThumbnail(const QString& thumbnail, const QString& filename);

private:
    QMap<QString, QString> files;
    bool shouldStop = false;
    QString cachePath;
};

class ThumbnailCreator : public QObject
{
    Q_OBJECT

public:
    ThumbnailCreator(QObject* parent = nullptr);
    virtual ~ThumbnailCreator();

    void start(const QMap<QString, QString>& files);

public slots:
    void handleOneDone(const QString& thumbnail);

signals:
    void operate();

private:
    QThread thread;
    Worker* worker;
    QStringList files;
};

#endif  // THUMBNAILCREATOR_H
