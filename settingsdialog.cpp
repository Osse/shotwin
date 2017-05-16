#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
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
    QSettings settings;
    settings.setValue("shade", ui->sliderBgColor->value());
    settings.setValue("cachepath", ui->leCachePath->text());
    emit settingsChanged();
}

void SettingsDialog::revert()
{
    emit bgColorChanged(QSettings().value("shade").toInt());
}
