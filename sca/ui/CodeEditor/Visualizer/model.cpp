#include "model.h"

CFGModel::CFGModel()
{

}

int CFGModel::rowCount(const QModelIndex &parent) const {
    return 1;
}

QVariant CFGModel::data(const QModelIndex &index, int role) const {
    QVariantMap dataMap;
    dataMap["xPos"] = 50;
    dataMap["yPos"] = 50;
    dataMap["width"] = 50;
    dataMap["height"] = 50;
    return dataMap;
}
