#include "infobox.h"
#include "ui_infobox.h"

InfoBox::InfoBox(QWidget* parent) : QWidget(parent), ui(new Ui::InfoBox)
{
    ui->setupUi(this);
    l = dynamic_cast<QGridLayout*>(layout());
}

InfoBox::~InfoBox()
{
    delete ui;
}

void InfoBox::showPhotoInfo(const PhotoInfo& info)
{
    setInfo(0, tr("Title:"), info.title);
    setInfo(1, tr("Date:"), info.exposureTime.date().toString());
    setInfo(2, tr("Time:"), info.exposureTime.time().toString());
    setInfo(3, tr("Size:"), QString("%1 x %2").arg(info.size.width()).arg(info.size.height()));
    setInfo(4, tr("Exposure:"), info.exposureString);
}

void InfoBox::showEventInfo(const EventInfo& info)
{
    setInfo(0, tr("Title:"), info.title);
    setInfo(1, tr("Items:"), QString::number(info.items));

    if (info.from.date().day() != info.to.date().day()) {
        setInfo(2, tr("From:"), info.from.toString());
        setInfo(3, tr("To:"), info.to.toString());
        setInfo(4, QString(), QString());
    }
    else {
        setInfo(2, tr("Date:"), info.from.date().toString());
        setInfo(3, tr("From:"), info.from.time().toString());
        setInfo(4, tr("To:"), info.to.time().toString());
    }
}

void InfoBox::showDirectoryInfo(DirectoryInfo info)
{
}

void InfoBox::setInfo(int row, const QString& title, const QString& value)
{
    label(row, 0)->setText(title);
    label(row, 1)->setText(value);
}

QLabel* InfoBox::label(int row, int column)
{
    return dynamic_cast<QLabel*>(l->itemAtPosition(row, column)->widget());
}
