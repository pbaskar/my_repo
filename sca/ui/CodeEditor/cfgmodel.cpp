#include "cfgmodel.h"
#include <QModelIndex>
#include <QStringList>
#include "cemodel.h"
#include "basicblock.h"
#include "positionblock.h"

CFGModel::CFGModel(QObject* parent) : QAbstractListModel(parent)
{
    CEModel* model = CEModel::getInstance();
    connect(model, &CEModel::CFGAvailable, this, &CFGModel::onCFGAvailable);
    //model->sendCommand("getCFG");
}

int CFGModel::rowCount(const QModelIndex &parent) const {
    return p_cfgList.size();
}

QVariant CFGModel::data(const QModelIndex &index, int role) const
{
    if(p_cfgList.empty())
        return QVariant();
    const auto positionBlock = p_cfgList.at(index.row());
    qDebug() << index.row();
    QVariantMap dataMap;
    dataMap["xPos"] = positionBlock.getX();
    dataMap["yPos"] = positionBlock.getY();
    dataMap["width"] = positionBlock.getWidth();
    dataMap["height"] = positionBlock.getHeight();
    QStringList stmts = positionBlock.getBlock()->getStmts();
    if (stmts.empty()) {
        QString title = positionBlock.getBlock()->getTitle();
        if (title.isEmpty()) {
            stmts << "dummy block";
        }
        else {
            stmts << title;
        }
    }
    dataMap["stmtList"] = stmts;
    dataMap["fullHeight"] = totalHeight();
    return dataMap;
}

void CFGModel::onCFGAvailable(const QList<PositionBlock> cfgList)
{
    p_cfgList = cfgList;
    qDebug()<< Q_FUNC_INFO<<p_cfgList.size();
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
