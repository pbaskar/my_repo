#ifndef CFGEDGE_H
#define CFGEDGE_H
#include <QQuickItem>


class CFGEdge : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
public:
    CFGEdge();
    virtual QSGNode* updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData);
private:
    QPoint p1;
    QPoint p2;
};

#endif // CFGEDGE_H
