#include "dp_device.h"

Dp_device::Dp_device(int id_, int rev_no_, int no_do_, int no_obj_, int fst_idx_, int no_typ_) :

    id(id_), rev_no(rev_no_),no_do(no_do_), no_obj(no_obj_), fst_idx(fst_idx_), no_typ(no_typ_)
{}


int Dp_device::getId() const
{
return id;
}

void Dp_device::setId(int value)
{
id = value;
}

int Dp_device::getRev_no() const
{
return rev_no;
}

void Dp_device::setRev_no(int value)
{
rev_no = value;
}

int Dp_device::getNo_do() const
{
return no_do;
}

void Dp_device::setNo_do(int value)
{
no_do = value;
}

int Dp_device::getNo_obj() const
{
return no_obj;
}

void Dp_device::setNo_obj(int value)
{
no_obj = value;
}

int Dp_device::getFst_idx() const
{
return fst_idx;
}

void Dp_device::setFst_idx(int value)
{
fst_idx = value;
}

int Dp_device::getNo_typ() const
{
return no_typ;
}

void Dp_device::setNo_typ(int value)
{
no_typ = value;
}
