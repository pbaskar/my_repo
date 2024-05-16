#include "edge.h"
#include <QDebug>

Edge::Edge(const Side sourceSide, const Side destinationSide, const PositionBlock sourceBlock, PositionBlock destinationBlock)
    : p_sourceSide(sourceSide), p_destinationSide(destinationSide), p_sourceBlock(sourceBlock), p_destinationBlock(destinationBlock)
{

}

const QPoint Edge::getSource()
{
    if(p_sourceSide == Side::BOTTOM)
        return p_sourceBlock.getBottomConnection();
    return p_sourceBlock.getTopConnection();
}

const QPoint Edge::getDestination()
{
    if(p_destinationSide == Side::BOTTOM)
        return p_destinationBlock.getBottomConnection();
    return p_destinationBlock.getTopConnection();
}

const QPoint Edge::getIntermediatePoint1()
{
    QPoint source;
    if(p_sourceSide == Side::TOP)
    {
        source = p_sourceBlock.getTopConnection();
        qDebug() <<Q_FUNC_INFO;
    }
    else if(p_sourceSide == Side::BOTTOM)
    {
        source = p_sourceBlock.getBottomConnection();
    }
    return QPoint(source.x(), source.y()+25);
}

const QPoint Edge::getIntermediatePoint2()
{
    QPoint destination;
    if(p_destinationSide == Side::TOP)
    {
        destination = p_destinationBlock.getTopConnection();
    }
    else if(p_destinationSide == Side::BOTTOM)
    {
        destination = p_destinationBlock.getBottomConnection();
    }
    return QPoint(destination.x(),destination.y()-25);
}
