#include "settingsdialog.h"
#include "ui_settingsdialog.h"

#include <QDialogButtonBox>
#include <QFileDialog>
#include <QPushButton>
#include <QSettings>

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::store);
    connect(ui->pbBrowseForCache, &QPushButton::clicked, this, &SettingsDialog::chooseCachePath);

    QSettings settings;

    QString cachePath = settings.contains("cachepath") ? settings.value("cachepath").toString() : "";
    ui->leCachePath->setText(cachePath);

    int shade = settings.contains("shade") ? settings.value("shade").toInt() : 128;
    ui->sliderBgColor->setValue(shade);
    connect(ui->sliderBgColor, &QSlider::valueChanged, this, &SettingsDialog::bgColorChanged);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::chooseCachePath()
{
    auto newPath = QFileDialog::getExistingDirectory(this, "Cache path");
    ui->leCachePath->setText(newPath);
}

void SettingsDialog::store()
{
    QSettings settings;
    settings.setValue("shade", ui->sliderBgColor->value());
    settings.setValue("cachepath", ui->leCachePath->text());
    emit settingsChanged();
}
