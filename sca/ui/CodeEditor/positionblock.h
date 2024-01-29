#ifndef POSITIONBLOCK_H
#define POSITIONBLOCK_H

class BasicBlock;
class PositionBlock
{
public:
    PositionBlock();
    PositionBlock(int x, int y, int width, int height, const BasicBlock* basicBlock);
    int getY() const { return p_y; }
    int getX() const { return p_x; }
    int getHeight() const { return p_height; }
    int getWidth() const { return p_width; }
    const BasicBlock* getBlock() const { return p_basicBlock; }
private:
    int p_x;
    int p_y;
    int p_width;
    int p_height;
    const BasicBlock* p_basicBlock;
};

#endif // POSITIONBLOCK_H
