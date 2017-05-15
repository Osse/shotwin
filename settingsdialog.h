#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui
{
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = 0);
    ~SettingsDialog();

signals:
    void settingsChanged();
    void bgColorChanged();

private:
    void chooseCachePath();
    void store();

private:
    Ui::SettingsDialog* ui;
};

#endif  // SETTINGSDIALOG_H
