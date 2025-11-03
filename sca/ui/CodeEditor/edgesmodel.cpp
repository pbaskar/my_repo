#include "edgesmodel.h"
#include <QModelIndex>
#include "cemodel.h"

EdgesModel::EdgesModel()
{
    CEModel* model = CEModel::getInstance();
    connect(model, &CEModel::edgesAvailable, this, &EdgesModel::onEdgesAvailable);
    //model->sendCommand("getCFG");
}

int EdgesModel::rowCount(const QModelIndex &parent) const {
    return p_edgesList.size();
}

QVariant EdgesModel::data(const QModelIndex &index, int role) const
{
    if(p_edgesList.empty())
        return QVariant();
    const auto edge = p_edgesList.at(index.row());
    qDebug() << index.row();
    QVariantMap dataMap;
    dataMap["point1"] = edge.getSource();
    dataMap["point2"] = edge.getIntermediatePoint1();
    dataMap["point3"] = edge.getIntermediatePoint2();
    dataMap["point4"] = edge.getDestination();
    return dataMap;
}

void EdgesModel::onEdgesAvailable(const QList<Edge> edgesList)
{
    p_edgesList = edgesList;
    qDebug()<< Q_FUNC_INFO<<p_edgesList.size();
    emit layoutChanged();
    /*QStandardItemModel* model = new QStandardItemModel(results.size(),0,this);
    CEOutputViewDelegate* delegate = new CEOutputViewDelegate(p_outputView);
    for(int i=0; i<model->rowCount(); i++)
    {
        QVariantMap map = results[i].toMap();
        qDebug() <<map["errorMessage"].toString();
        QStandardItem *item = new QStandardItem();
        item->setData(map["errorMessage"].toString(), Qt::DisplayRole);
        model->setItem(i, 0, item);
    }
    p_outputView->setModel(model);
    p_outputView->setItemDelegate(delegate);*/
}
