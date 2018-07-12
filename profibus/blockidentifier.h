#ifndef BLOCKIDENTIFIER_H
#define BLOCKIDENTIFIER_H
#include <QList>


class blockIdentifier
{
public:
    blockIdentifier();
    QList<QString> getBlockIdentity (int blocknumber, int parent_class_number, int class_number) ;
    QList<QString> getParentClassFB( int parentClass, int classNumber);
    QList<QString> getClassFB(int classNumber);
    QList<QString> getParentClassTB(  int parentClass, int classNumber);
};

#endif // BLOCKIDENTIFIER_H
