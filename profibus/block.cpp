#include "block.h"

Block::Block()
{

}

int Block::getSlot() const
{
    return slot;
}

void Block::setSlot(int value)
{
    slot = value;
}

int Block::getIndex() const
{
    return index;
}

void Block::setIndex(int value)
{
    index = value;
}

