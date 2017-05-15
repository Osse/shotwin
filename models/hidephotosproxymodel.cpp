#include "hidephotosproxymodel.h"

#include "eventtreeitem.h"
#include "eventitem.h"

#include <QIdentityProxyModel>

HidePhotosProxyModel::HidePhotosProxyModel(QObject* parent) : QIdentityProxyModel(parent)
{
}

bool HidePhotosProxyModel::hasChildren(const QModelIndex& parent) const
{
    if (!parent.isValid())
        return sourceModel()->hasChildren(parent);

    auto item = static_cast<EventTreeItem*>(parent.internalPointer());

    if (dynamic_cast<EventItem*>(item))
        return false;
    else
        return sourceModel()->hasChildren(parent);
}
