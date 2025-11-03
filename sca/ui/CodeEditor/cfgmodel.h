#ifndef CFGMODEL_H
#define CFGMODEL_H
#include <QFile>
#include <QList>
#include <QAbstractListModel>
#include "qqmlintegration.h"
#include "positionblock.h"

class CFGModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(int totalHeight READ totalHeight NOTIFY totalHeightChanged);
    Q_PROPERTY(int totalWidth READ totalWidth NOTIFY totalWidthChanged);
public:
    CFGModel(QObject* parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    int totalHeight() const {
        int h = p_cfgList.constLast().getY() + p_cfgList.constLast().getHeight();
        /*QFile file("C:\\workspace\\my_repo\\sca\\test\\ui.log");
        file.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Append);
        QTextStream text(&file);
        text << "cfgmodel ::totalHeight x, y " <<h << Qt::endl;
        file.close();*/
        return h;
    }
    int totalWidth() { return p_cfgList.constLast().getX() + p_cfgList.constLast().getWidth(); }
signals:
    void totalHeightChanged();
    void totalWidthChanged();
public slots:
    void onCFGAvailable(const QList<PositionBlock> cfgList);
private:
    QList<PositionBlock> p_cfgList;
};

#endif // CFGMODEL_H
