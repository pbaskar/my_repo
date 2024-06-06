#ifndef EDGE_H
#define EDGE_H
#include "positionblock.h"

enum Side {
    TOP,
    BOTTOM
};

class Edge
{
public:
    Edge(const Side sourceSide, const Side destinationSide, const PositionBlock sourceBlock, const PositionBlock destinationBlock);
    const QPoint getSource() const;
    const QPoint getDestination() const;
    const QPoint getIntermediatePoint1() const;
    const QPoint getIntermediatePoint2() const;
private:
    const PositionBlock p_sourceBlock;
    const PositionBlock p_destinationBlock;
    const Side p_sourceSide;
    const Side p_destinationSide;
};

#endif // EDGE_H
