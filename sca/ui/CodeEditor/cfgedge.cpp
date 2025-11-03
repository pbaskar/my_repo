#include "cfgedge.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSurfaceFormat>

static const int SEGMENT_COUNT = 32;
static const float SEGMENT_LEN = 1/SEGMENT_COUNT;
static const int ARROW_COUNT = 6;
static const int CIRCLE_COUNT = 36;
static const int VERTICES_COUNT = CIRCLE_COUNT + SEGMENT_COUNT + ARROW_COUNT;
static const int CIRCLE_RADIUS = 1;
CFGEdge::CFGEdge()
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setSamples(8);
    QSurfaceFormat::setDefaultFormat(format);
    setFlag(QQuickItem::ItemHasContents, true);
}

QSGNode* CFGEdge::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    QSGGeometryNode* lineNode = nullptr;
    QSGGeometry* lineGeometry = nullptr;
    QSGGeometryNode* arrowNode = nullptr;
    QSGGeometry* arrowGeometry = nullptr;
    if(!oldNode)
    {
        lineNode = new QSGGeometryNode;

        lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), VERTICES_COUNT);
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLineStrip);
        lineGeometry->setLineWidth(20);

        auto *lineMaterial = new QSGFlatColorMaterial;
        lineMaterial->setColor(QColor(0, 116, 0));
        lineNode->setMaterial(lineMaterial);
        lineNode->setFlag(QSGNode::OwnsMaterial);
    }
    else
    {
        lineNode = static_cast<QSGGeometryNode*>(oldNode->childAtIndex(0));
        lineGeometry = lineNode->geometry();
        lineGeometry->allocate(VERTICES_COUNT);
    }

    QSGGeometry::Point2D* lineVertices = lineGeometry->vertexDataAsPoint2D();
    QPoint center = {p1.x(), p1.y()};
    int next =0;
    int degrees = 90;
    lineVertices[next++].set(p1.x(), p1.y()+ CIRCLE_RADIUS);
    for(int i=1; i<CIRCLE_COUNT; i++, degrees+= 10)
    {
        if(degrees ==360) degrees=0;
        int radians = 3.14/180*degrees;
        lineVertices[next++].set(p1.x()+CIRCLE_RADIUS*qCos(radians), p1.y()+CIRCLE_RADIUS*qSin(radians));
    }
    float t=0.03125;
    float x=0, y=0;
    lineVertices[next++].set(p1.x(), p1.y()+ CIRCLE_RADIUS);
    for(int i=1; i<SEGMENT_COUNT; i++, t+=0.03125)
    {
        x = qPow(1-t,3) * p1.x() + 3*t*qPow(1-t,2) * p2.x() + 3*qPow(t,2) * (1-t) * p3.x() + qPow(t,3) * p4.x();
        y = qPow(1-t,3) * p1.y() + 3*t*qPow(1-t,2) * p2.y() + 3*qPow(t,2) * (1-t) * p3.y() + qPow(t,3) * (p4.y()-10);
        qDebug() <<Q_FUNC_INFO <<"i = " <<i << "t = " <<t <<"x = "<<x <<"y = " <<y;
        lineVertices[next++].set(x,y);
    }

    t = 1-0.03125;
    float dx = 3*qPow(1-t,2)* p1.x() + 3 * t * 2*(1-t) *p2.x() + 3 * qPow(1-t,2)* p2.x() +6*t*(1-t) +3*qPow(t,2) * -1 * p3.x() +3*qPow(t,2)* p4.x();
    float dy = 3*qPow(1-t,2)* p1.y() + 3 * t * 2*(1-t) *p2.y() + 3 * qPow(1-t,2)* p2.y() +6*t*(1-t) +3*qPow(t,2) * -1 * p3.y() +3*qPow(t,2)* (p4.y()-10);

    lineVertices[next++].set(p4.x(), p4.y()-10);
    lineVertices[next++].set(p4.x(), p4.y()-5);
    lineVertices[next++].set(p4.x()-5, p4.y()-5);
    lineVertices[next++].set(p4.x(), p4.y());
    lineVertices[next++].set(p4.x()+5, p4.y()-5);
    lineVertices[next++].set(p4.x(), p4.y()-5);

    lineNode->markDirty(QSGNode::DirtyGeometry);
    lineNode->markDirty(QSGNode::DirtyMaterial);
    return lineNode;
}
