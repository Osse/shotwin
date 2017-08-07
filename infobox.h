#ifndef INFOBOX_H
#define INFOBOX_H

#include <QDateTime>
#include <QWidget>

struct PhotoInfo {
    QString title;
    QSize size;
    QDateTime exposureTime;
    QString exposureString;
};

struct EventInfo {
    QString title;
    int items;
    QDateTime from;
    QDateTime to;
};

struct DirectoryInfo {
    int eventItems;
    int items;
    QDateTime from;
    QDateTime to;
};

namespace Ui
{
class InfoBox;
}

class QGridLayout;
class QLabel;

class InfoBox : public QWidget
{
    Q_OBJECT

public:
    explicit InfoBox(QWidget* parent = 0);
    ~InfoBox();

    void showPhotoInfo(const PhotoInfo& info);
    void showEventInfo(const EventInfo& info);
    void showDirectoryInfo(DirectoryInfo info);

private:
    void setInfo(int row, const QString& title, const QString& value);
    QLabel* label(int row, int column);

private:
    Ui::InfoBox* ui;
    QGridLayout* l;
};

#endif  // INFOBOX_H
