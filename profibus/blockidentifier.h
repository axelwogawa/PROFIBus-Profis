#ifndef BLOCKIDENTIFIER_H
#define BLOCKIDENTIFIER_H
#include <QList>


class blockIdentifier
{
public:
    blockIdentifier();
    QList<QString> getBlockIdentity (int blocknumber, int parent_class_number, int class_number) ;
    QList<QString> getParentClassFB( int parentClass, int classNumber);
    QList<QString> get_AI_ClassFB(int classNumber);
    QList<QString> getParentClassTB(  int parentClass, int classNumber);
    QList<QString> get_Control_ClassFB(int classNumber);
    QList<QString> getParentClassPB(int parentClass, int classNumber);
    QList<QString> getChildClassTBPress(int classNumber);
    QList<QString> getChildClassTBTemp(int classNumber);
    QList<QString> getChildClassTBDummy(int classNumber);
    QList<QString> get_ChildClassFBAux(int classNumber);
};

#endif // BLOCKIDENTIFIER_H
