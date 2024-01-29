#ifndef MODEL_H
#define MODEL_H

#include "qqmlintegration.h"
#include <QAbstractListModel>

class CFGModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    CFGModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
};

#endif // MODEL_H
