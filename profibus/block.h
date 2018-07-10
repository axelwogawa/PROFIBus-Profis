#ifndef BLOCK_H
#define BLOCK_H


class Block
{
public:
    Block();

    int getSlot() const;
    void setSlot(int value);

    int getIndex() const;
    void setIndex(int value);

private:
    int slot;
    int index;
};

#endif // BLOCK_H
