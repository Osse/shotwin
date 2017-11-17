#include "modelviewdialog.h"
#include "ui_modelviewdialog.h"

#include "datefilteredeventmodel.h"
#include "eventmodel.h"
#include "eventortagfilteredphotomodel.h"
#include "photomodel.h"
#include "shotwin.h"
#include "treeproxymodel.h"

#include <QAbstractItemModel>
#include <QLabel>
#include <QListView>
#include <QPushButton>
#include <QShortcut>
#include <QTabWidget>
#include <QTreeView>

ModelViewDialog::ModelViewDialog(Shotwin* shotwin, QWidget* parent)
    : QDialog(parent), ui(new Ui::ModelViewDialog), shotwin(shotwin)
{
    ui->setupUi(this);

    ui->eventTreeView->setModel(shotwin->getEventTree());
    ui->tagTreeView->setModel(shotwin->getTagModel());

    auto expandShortcut = new QShortcut(Qt::Key_F2, this);
    connect(expandShortcut, &QShortcut::activated, ui->eventTreeView, &QTreeView::expandAll);
}

ModelViewDialog::~ModelViewDialog()
{
    delete ui;
}
