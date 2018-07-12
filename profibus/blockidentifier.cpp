#include "blockidentifier.h"
#include <QString>

blockIdentifier::blockIdentifier()
{

}

QList<QString> blockIdentifier::getBlockIdentity (int blocknumber, int parent_class_number, int class_number) {
    QList<QString> identityList;
    switch (blocknumber) {
    case 1:
   // identityList = getParentClassPB(parent_class_number,class_number);
    identityList.append("PB");
        break;
    case 2:
       identityList= getParentClassFB(parent_class_number,class_number);
       identityList.append("FB");

        break;
    case 3:
     identityList.append("TB");
    // to be continued here
        break;

    default:
        break;
    }
    printf("%3d",identityList.size());
    return identityList;
}

QList<QString> blockIdentifier::getParentClassFB(  int parentClass, int classNumber){
    QList<QString> list;
    switch (parentClass) {
    case 1:
         list = getClassFB(classNumber);
         list.append( "Input");
         break;
    case 2:   list = getClassFB(classNumber);
        list.append( "Output");
        break;
    case 3:   list = getClassFB(classNumber);
        list.append("Control");
        break;
    case 4:   list = getClassFB(classNumber);
        list.append( "Advanced Control");
        break;
    case 5:   list = getClassFB(classNumber);
        list.append( "Calculation");
        break;
    case 6: list = getClassFB(classNumber);
        list.append( "Auxiliary");
        break;
    case 7:list = getClassFB(classNumber);
        list.append( "Alert");
        break;

  default:
        list = getClassFB(classNumber);
                list.append( "special");
        break;
    }
      return list;

}

QList<QString> blockIdentifier::getClassFB(int classNumber){
    QList<QString> identityList;
    switch (classNumber) {
    case 1: identityList.append("Analog Input");
         printf("%3d",identityList.size());
         return identityList;
        break;
    case 2: identityList.append("Discrete Input");
        break;

  default:identityList.append("special");
        break;
    }
}


//QList<QString> blockIdentifier::getParentClassTB(  int parentClass, int classNumber){}
/* Transducer
switch (classNumber) {
case 1: identityList.append("");
     printf("%3d",identityList.size());
     return identityList;
    break;
case 2: identityList.append("Absolute");
    break;
case 3: identityList.append("Gage");
    break;
case 4: identityList.append("Pressure");
    break;
case 5: identityList.append("Actuator");
    break;
case 6: identityList.append("Discrete_I_O");
    break;
case 7:identityList.append("Analyzer");
    break;
case 8: identityList.append("Auxiliary");
    break;
case 9: identityList.append("Alarm");
default:
    break;
} */
