#ifndef CFGMODEL_H
#define CFGMODEL_H
#include <QAbstractListModel>
#include "qqmlintegration.h"
#include "positionblock.h"

class CFGModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    CFGModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
public slots:
    void onCFGAvailable(const QList<PositionBlock> cfgList);
private:
    QList<PositionBlock> p_cfgList;
};

#endif // CFGMODEL_H
