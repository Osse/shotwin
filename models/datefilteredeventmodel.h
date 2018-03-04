#ifndef DATEFILTEREDEVENTMODEL_H
#define DATEFILTEREDEVENTMODEL_H

#include <QSortFilterProxyModel>

class DateFilteredEventModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    DateFilteredEventModel(QObject* parent = nullptr);

    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

    void setFilterYear(int value);
    void setFilterMonth(const int& year, const int& month);

    void resetFilter();

signals:
    void dateFilterChanged();

private:
    void invalidate();

private:
    int year = -1;
    int month = -1;
};

#endif  // DATEFILTEREDEVENTMODEL_H
