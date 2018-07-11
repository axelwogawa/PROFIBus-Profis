#ifndef BLOCK_H
#define BLOCK_H


class Block
{
public:
    Block();

    int getDo_index() const;
    void setDo_index(int value);

    int getDo_offset() const;
    void setDo_offset(int value);

    int getSlot() const;
    void setSlot(int value);

    int getIndex() const;
    void setIndex(int value);

    void setNo_params(int value);
    int getNo_params() const;

    int getView_slot() const;
    void setView_slot(int value);

    int getView_index() const;
    void setView_index(int value);

private:
    int do_index;   //index of the directory object, that references this block
    int do_offset;  //index of this block within the list of blocks of the same type
    int slot;
    int index;
    int no_params;  //number of parameters in this block

    //---values from block object---
    int view_slot;          //Slot of the block view
    int view_index;         //Index of the block view
};

#endif // BLOCK_H
