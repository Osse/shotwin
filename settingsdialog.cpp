#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QStandardPaths>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::store);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &SettingsDialog::revert);
    connect(ui->pbBrowseForFfmpeg, &QPushButton::clicked, this, &SettingsDialog::chooseFfmpegPath);

    QSettings settings;

    ui->leFfmpegCmd->setText(settings.value(QStringLiteral("ffmpegcmd"), "ffmpeg").toString());

    int shade = settings.value(QStringLiteral("shade"), 128).toInt();
    ui->sliderBgColor->setValue(shade);
    connect(ui->sliderBgColor, &QSlider::valueChanged, this, &SettingsDialog::bgColorChanged);

    auto map = settings.value(QStringLiteral("map")).toMap();

    QString str;
    for (auto it = map.begin(); it != map.end(); ++it) {
        str += it.key() + ";" + it.value().toString() + "\n";
    }

    ui->tePathMappings->setPlainText(str);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::chooseFfmpegPath()
{
    auto currentPath = ui->leFfmpegCmd->text();
    auto newPath =
        QFileDialog::getOpenFileName(this, tr("Path to ffmpeg executable"), currentPath, tr("Executables (*.exe)"));
    if (!newPath.isEmpty())
        ui->leFfmpegCmd->setText(newPath);
}

void SettingsDialog::store()
{
    QMap<QString, QVariant> map;
    for (const auto& line : ui->tePathMappings->toPlainText().split(QStringLiteral("\n"), QString::SkipEmptyParts)) {
        auto parts = line.split(QStringLiteral(";"));
        map[parts[0]] = parts[1];
    }

    QSettings settings;
    settings.setValue(QStringLiteral("shade"), ui->sliderBgColor->value());
    settings.setValue(QStringLiteral("ffmpegpath"), ui->leFfmpegCmd->text());
    settings.setValue(QStringLiteral("map"), map);

    emit settingsChanged();
}

void SettingsDialog::revert()
{
    emit bgColorChanged(QSettings().value(QStringLiteral("shade")).toInt());
}
