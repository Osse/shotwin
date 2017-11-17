#ifndef MODELVIEWDIALOG_H
#define MODELVIEWDIALOG_H

#include <QDialog>

class Shotwin;
class TreeProxyModel;
class EventModel;
class DateFilteredEventModel;

namespace Ui
{
class ModelViewDialog;
}

class ModelViewDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ModelViewDialog(Shotwin* shotwin, QWidget* parent = 0);
    ~ModelViewDialog();

private:
    void selectEvent(const QModelIndex& index);

private:
    Ui::ModelViewDialog* ui;
    Shotwin* shotwin = nullptr;

    TreeProxyModel* treeProxyModel = nullptr;
    TreeProxyModel* secondTreeProxyModel = nullptr;
    EventModel* eventModel = nullptr;
    DateFilteredEventModel* dateFilteredEventModel = nullptr;
};

#endif  // MODELVIEWDIALOG_H
