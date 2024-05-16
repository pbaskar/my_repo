#ifndef CFGEDGE_H
#define CFGEDGE_H
#include <QQuickItem>


class CFGEdge : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QPoint point1 READ point1 WRITE setPoint1);
    Q_PROPERTY(QPoint point2 READ point2 WRITE setPoint2);
    Q_PROPERTY(QPoint point3 READ point3 WRITE setPoint3);
    Q_PROPERTY(QPoint point4 READ point4 WRITE setPoint4);
public:
    CFGEdge();
    virtual QSGNode* updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData);
    void setPoint1(const QPoint& p) { p1 = p; }
    void setPoint2(const QPoint& p) { p2 = p; }
    void setPoint3(const QPoint& p) { p3 = p; }
    void setPoint4(const QPoint& p) { p4 = p; }
    const QPoint point1() { return p1; }
    const QPoint point2() { return p2; }
    const QPoint point3() { return p3; }
    const QPoint point4() { return p4; }
private:
    QPoint p1;
    QPoint p2;
    QPoint p3;
    QPoint p4;
};

#endif // CFGEDGE_H
