#include "positionblock.h"
#include "basicblock.h"

PositionBlock::PositionBlock() : p_x(0), p_y(0), p_width(0), p_height(0), p_basicBlock(0)
{

}

PositionBlock::PositionBlock(int x, int y, int width, int height, const BasicBlock* basicBlock)
    : p_x(x), p_y(y), p_width(width), p_height(height), p_basicBlock(basicBlock)
{

}
