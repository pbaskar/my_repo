#ifndef EDGESMODEL_H
#define EDGESMODEL_H
#include <QAbstractListModel>
#include "qqmlintegration.h"
#include "edge.h"

class PositionBlock;
class EdgesModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
public:
    EdgesModel();
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
public slots:
    void onEdgesAvailable(const QList<Edge> edgesList);
private:
    QList<Edge> p_edgesList;
};
#endif // EDGESMODEL_H
