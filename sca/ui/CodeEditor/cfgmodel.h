#ifndef CFGMODEL_H
#define CFGMODEL_H
#include <QAbstractListModel>
#include "qqmlintegration.h"
#include "positionblock.h"

class CFGModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int totalHeight READ getTotalHeight);
    Q_PROPERTY(int totalWidth READ getTotalWidth);
public:
    CFGModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    const int getTotalHeight() const { return p_cfgList.constLast().getHeight(); }
    const int getTotalWidth() const { return p_cfgList.constLast().getWidth(); }
public slots:
    void onCFGAvailable(const QList<PositionBlock> cfgList);
private:
    QList<PositionBlock> p_cfgList;
};

#endif // CFGMODEL_H
