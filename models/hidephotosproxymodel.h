#ifndef HIDEPHOTOSPROXYMODEL_H
#define HIDEPHOTOSPROXYMODEL_H

#include <QIdentityProxyModel>

class HidePhotosProxyModel : public QIdentityProxyModel
{
public:
    HidePhotosProxyModel(QObject* parent = nullptr);

    bool hasChildren(const QModelIndex& parent) const;
};

#endif // HIDEPHOTOSPROXYMODEL_H
