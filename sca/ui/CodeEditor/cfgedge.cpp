#include "cfgedge.h"
#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>

static const int VERTEX_COUNT = 2;
CFGEdge::CFGEdge()
{
    setFlag(QQuickItem::ItemHasContents, true);
    p1 = QPoint(50, 50);
    p2 = QPoint(100, 100);
}

QSGNode* CFGEdge::updatePaintNode(QSGNode *oldNode, QQuickItem::UpdatePaintNodeData *updatePaintNodeData)
{
    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;
    if(!oldNode)
    {
        node = new QSGGeometryNode;

        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), VERTEX_COUNT);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        geometry->setDrawingMode(QSGGeometry::DrawLineStrip);

        auto *material = new QSGFlatColorMaterial;
        material->setColor(QColor(255, 0, 0));
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);

    }
    else
    {
        node = static_cast<QSGGeometryNode*>(oldNode);
        geometry = node->geometry();
        geometry->allocate(VERTEX_COUNT);
    }

    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();
    vertices[0].set(p1.x(), p1.x());
    vertices[1].set(p2.x(), p2.y());
    node->markDirty(QSGNode::DirtyGeometry);
    node->markDirty(QSGNode::DirtyMaterial);
    return node;
}
