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
#include <QTabWidget>
#include <QTreeView>

std::vector<QVariant> getPhotoGroupingData(const QModelIndex& index)
{
    auto photoItem = static_cast<const PhotoItem*>(index.internalPointer());
    auto pathComponents = photoItem->getFilename().split("/", QString::SplitBehavior::SkipEmptyParts);
    pathComponents.pop_back();
    return std::vector<QVariant>(pathComponents.begin(), pathComponents.end());
}

std::vector<QVariant> getEventGroupingData(const QModelIndex& index)
{
    auto eventItem = static_cast<const EventItem*>(index.internalPointer());
    auto date = eventItem->getStartTime().date();
    return {date.toString("yyyy"), date.toString("MMMM")};
}

ModelViewDialog::ModelViewDialog(Shotwin* shotwin, QWidget* parent)
    : QDialog(parent), ui(new Ui::ModelViewDialog), shotwin(shotwin)
{
    ui->setupUi(this);

    eventModel = new EventModel(this);

    treeProxyModel = new TreeProxyModel(this);
    treeProxyModel->setGroupingDataCb(TreeProxyModel::GroupingDataCbType(getEventGroupingData));
    treeProxyModel->setSourceModel(eventModel);

    secondTreeProxyModel = new TreeProxyModel(this);
    secondTreeProxyModel->setGroupingDataCb(TreeProxyModel::GroupingDataCbType(getPhotoGroupingData));
    secondTreeProxyModel->setSourceModel(shotwin->getPhotoModel());

    dateFilteredEventModel = new DateFilteredEventModel(this);
    dateFilteredEventModel->setSourceModel(eventModel);

    QAbstractItemModel* models[] = {eventModel,
                                    dateFilteredEventModel,
                                    shotwin->getEventTree(),
                                    shotwin->getPhotoList(),
                                    shotwin->getPhotoModel(),
                                    treeProxyModel,
                                    secondTreeProxyModel};

    QAbstractItemView* views[] = {new QListView(),
                                  new QListView(),
                                  new QTreeView(),
                                  new QListView(),
                                  new QListView(),
                                  new QTreeView(),
                                  new QTreeView()};

    auto button = ui->buttonBox->addButton(QDialogButtonBox::StandardButton::Reset);

    for (int i = 0; i < 7; i++) {
        QAbstractItemView* view = views[i];
        QAbstractItemModel* model = models[i];
        auto widget = new QWidget();
        auto layout = new QVBoxLayout(widget);

        view->setModel(model);
        layout->addWidget(view);

        auto rowCountLabel = new QLabel();
        rowCountLabel->setText("rowCount: " + QString::number(model->rowCount(QModelIndex())));
        layout->addWidget(rowCountLabel);

        connect(button, &QPushButton::clicked, this, [=]() {
            view->clearSelection();
            view->setCurrentIndex(QModelIndex());
            rowCountLabel->setText("rowCount: " + QString::number(model->rowCount(QModelIndex())));
        });

        ui->tabWidget->addTab(widget, QString::number(i) + ": " + model->metaObject()->className());
        connect(view, &QAbstractItemView::clicked, [=](const QModelIndex& index) {
            rowCountLabel->setText("rowCount: " + QString::number(model->rowCount(index)));
        });
    }

    connect(views[5], &QTreeView::clicked, this, &ModelViewDialog::selectEvent);
}

ModelViewDialog::~ModelViewDialog()
{
    delete ui;
}

void ModelViewDialog::selectEvent(const QModelIndex& index)
{
    auto sourceIndex = treeProxyModel->mapToSource(index);
    if (sourceIndex.isValid()) {
        auto item = static_cast<EventItem*>(sourceIndex.internalPointer());
        dynamic_cast<EventOrTagFilteredPhotoModel*>(shotwin->getPhotoList())->setEventId(item->getEventId());
    }
    else {
        if (index.parent().isValid()) {
            int year = treeProxyModel->data(index.parent()).toInt();
            auto tempDate = treeProxyModel->data(index).toString();
            int month = QDate::fromString(tempDate, "MMMM").month();
            dateFilteredEventModel->setFilterMonth(year, month);
        }
        else {
            int year = treeProxyModel->data(index).toInt();
            dateFilteredEventModel->setFilterYear(year);
        }
    }
}
