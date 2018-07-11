#ifndef PB_H
#define PB_H
#include "block.h"
#include "QString"


class PB : public Block
{
public:
    PB();

    QString getCls() const;
    void setCls(const QString &value);

private:
    QString cls;    //device class
};

#endif // PB_H
