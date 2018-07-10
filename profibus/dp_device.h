#ifndef DP_DEVICE_H
#define DP_DEVICE_H
#include "block.h"
#include "pb.h"
#include "tb.h"
#include "fb.h"
#include "vector"


class Dp_device
{
public:
    Dp_device(int id_, int rev_no_, int no_do_, int no_obj_, int fst_idx_, int no_typ_);
    int getId() const;
    void setId(int value);

    int getRev_no() const;
    void setRev_no(int value);

    int getNo_do() const;
    void setNo_do(int value);

    int getNo_obj() const;
    void setNo_obj(int value);

    int getFst_idx() const;
    void setFst_idx(int value);

    int getNo_typ() const;
    void setNo_typ(int value);

    PB& getPb();
    void setPb(PB &value);

    TB& getTb(int index);
    std::vector<TB>& getTbs();
    void setTb(TB &value, int index);

    FB& getFb(int index);
    std::vector<FB>& getFbs();
    void setFb(FB &value, int index);

private:
    //---values from directory object header---
    int id;         //device ID (=Dir_ID)
    int rev_no;     //revision number (=Rev-Number)
    int no_do;      //number of directory objects (=Num_Dir_Obj)
    int no_obj;     //number of objects in this device (=Num_Dir_Entry-Num_Comp_List_Dir_Entry)
    int fst_idx;    //index of the first object (=Fist_Comp_List_Dir_Entry)
    int no_typ;     //number of object types: phys/func/transd (=Num_Comp_List_Dir_Entry)

    //---values from directory object---
    PB pb;                  //the physical block object
    std::vector<TB> tbs;    //vector of transducer block objects
    std::vector<FB> fbs;    //vector of function block objects
};

#endif // DP_DEVICE_H
