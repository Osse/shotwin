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
    connect(ui->pbBrowseForCache, &QPushButton::clicked, this, &SettingsDialog::chooseCachePath);

    QSettings settings;

    ui->leCachePath->setText(settings.value("cachepath", QString()).toString());

    int shade = settings.value("shade", 128).toInt();
    ui->sliderBgColor->setValue(shade);
    connect(ui->sliderBgColor, &QSlider::valueChanged, this, &SettingsDialog::bgColorChanged);

    auto map = settings.value("map").toMap();

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

void SettingsDialog::chooseCachePath()
{
    auto currentPath = ui->leCachePath->text();
    auto newPath = QFileDialog::getExistingDirectory(this, tr("Path to existing cache"), currentPath);
    if (!newPath.isEmpty())
        ui->leCachePath->setText(newPath);
}

void SettingsDialog::store()
{
    QMap<QString, QVariant> map;
    for (const auto& line : ui->tePathMappings->toPlainText().split("\n", QString::SkipEmptyParts)) {
        auto parts = line.split(";");
        map[parts[0]] = parts[1];
    }

    QSettings settings;
    settings.setValue("shade", ui->sliderBgColor->value());
    settings.setValue("cachepath", ui->leCachePath->text());
    settings.setValue("map", map);

    emit settingsChanged();
}

void SettingsDialog::revert()
{
    emit bgColorChanged(QSettings().value("shade").toInt());
}
