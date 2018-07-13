#include "blockidentifier.h"
#include <QString>

blockIdentifier::blockIdentifier()
{

}



// Block identifier according page 48
//output: list[<class>,<parent_class>,<block_type>]
QList<QString> blockIdentifier::getBlockIdentity (int blocknumber, int parent_class_number, int class_number) {
    QList<QString> identityList;
    switch (blocknumber) {
    case 1:
    identityList = getParentClassPB(parent_class_number,class_number);
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


//################## FB ####################

QList<QString> blockIdentifier::getParentClassFB(  int parentClass, int classNumber){
    QList<QString> list;
    switch (parentClass) {
    case 1:
         list = get_AI_ClassFB(classNumber);
         list.append( "Input");
         break;
    case 2:   list = get_AI_ClassFB(classNumber);
        list.append( "Output");
        break;
    case 3:   list = get_Control_ClassFB(classNumber);
        list.append("Control");
        break;
    case 4:   list = get_AI_ClassFB(classNumber);
        list.append( "Advanced Control");
        break;
    case 5:   list = get_AI_ClassFB(classNumber);
        list.append( "Calculation");
        break;
    case 6: list = get_AI_ClassFB(classNumber);
        list.append( "Auxiliary");
        break;
    case 7:list = get_AI_ClassFB(classNumber);
        list.append( "Alert");
        break;

  default:
        list = get_AI_ClassFB(classNumber);
                list.append( "reserved");
        break;
    }
      return list;
}


QList<QString> blockIdentifier::get_Control_ClassFB(int classNumber){
    QList<QString> identityList;
    switch (classNumber) {
    case 1: identityList.append("PID");
         return identityList;
        break;
    case 2: identityList.append("SampleSelector");
        break;
    case 3: identityList.append("LabDeviceCOntrol");
        break;

  default:identityList.append("reserved");
        break;
    }

    return identityList;
}


QList<QString> blockIdentifier::get_AI_ClassFB(int classNumber){
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

    return identityList;
}


//################## TB ####################

QList<QString> blockIdentifier::getParentClassTB(  int parentClass, int classNumber)
{
    QList<QString> identityList;

    switch (parentClass) {
    case 1: identityList = getChildClassTBPress(classNumber);
        identityList.append("Pressure");
         /*printf("%3d",identityList.size());
         return identityList;*/
        break;
    case 2: identityList = getChildClassTBTemp(classNumber);
        identityList.append("Temperature");
        break;
    case 3: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Flow");
        break;
    case 4: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Level");
        break;
    case 5: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Actuator");
        break;
    case 6: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Discrete_I_O");
        break;
    case 7:identityList = getChildClassTBDummy(classNumber);
        identityList.append("Analyzer");
        break;
    case 8: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Auxiliary");
        break;
    case 9: identityList = getChildClassTBDummy(classNumber);
        identityList.append("Alarm");
    default:
        identityList.append("unknown");
        identityList.append("reserved");
        break;
    }

    return identityList;
}


QList<QString> blockIdentifier::getChildClassTBPress(int classNumber)
{
    QList<QString> identityList;

    switch (classNumber) {
    case 1: identityList.append("Differential");
        break;
    case 2: identityList.append("Absolute");
        break;
    case 3: identityList.append("Gage");
        break;
    case 4: identityList.append("Pressure + Level + Flow");
        break;
    case 5: identityList.append("Actuator");
        break;
    case 6: identityList.append("Pressure + Flow");
        break;
    case 7:identityList.append("Mixed absolute / diff. Pressure");
        break;
    default:
        identityList.append("reserved");
        break;
    }

    return identityList;
}


QList<QString> blockIdentifier::getChildClassTBTemp(int classNumber)
{
    QList<QString> identityList;

    switch (classNumber) {
    case 1: identityList.append("Thermocouple");
        break;
    case 2: identityList.append("Resistance Thermometer");
        break;
    case 3: identityList.append("Pyrometer");
        break;
    case 16: identityList.append("TC + DC U (DC Voltage)");
        break;
    case 17: identityList.append("RTD + R (R-Resistance)");
        break;
    case 18: identityList.append("TC+RTD+R+DC U");
        break;
   default:
        identityList.append("reserved");
        break;
    }

    return identityList;
}


QList<QString> blockIdentifier::getChildClassTBDummy(int classNumber)
{
    QList<QString> identityList;

    switch (classNumber) {
    default:
        identityList.append("R2D2");
        break;
    }

    return identityList;
}



//################## PB ####################

QList<QString> blockIdentifier::getParentClassPB(  int parentClass, int classNumber)
{
    QList<QString> identityList;

    //parse subclass
    if (classNumber == 250)
        identityList.append("not used");
    else
        identityList.append("unknown");

    //parse parent class
    switch(parentClass) {
    case 1:
        identityList.append("Transmitter");
        break;
    case 2:
        identityList.append("Actuator");
        break;
    case 3:
        identityList.append("Discrete I/O");
        break;
    case 4:
        identityList.append("Controller");
        break;
    case 5:
        identityList.append("Analyser");
        break;
    case 6:
        identityList.append("Lab Device");
        break;
    default:
        if(parentClass>=128 && parentClass <= 249)
            identityList.append("vendor specific");
        else
            identityList.append("unknown");
        break;
    }

    return identityList;
}


