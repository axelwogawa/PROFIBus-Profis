#include "block.h"

Block::Block()
{

}

int Block::getDo_index() const
{
    return do_index;
}

void Block::setDo_index(int value)
{
    do_index = value;
}

int Block::getDo_offset() const
{
    return do_offset;
}

void Block::setDo_offset(int value)
{
    do_offset = value;
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

void Block::setNo_params(int value)
{
    no_params = value;
}

int Block::getNo_params() const
{
    return no_params;
}

