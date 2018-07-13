#include "parser.h"
#include "QString"


Parser::Parser()
{

}


//parsing engineering unit from data type DS-36 (page 63, 97)
QString Parser::parseDS36Unit(std::vector<int> in){
    std::vector<int> units_idx;
    units_idx.push_back(in.at(8));
    units_idx.push_back(in.at(9));
    int id = US16toInt(units_idx);
    return parseUnitId(id);
}


QString Parser::parseUnitId(int id){
    switch(id)
    {
    case 1000:
        return QString("Kelvin");
        break;
    case 1001:
        return QString("Degree Celsius");
        break;
    case 1002:
        return QString("Degree Fahrenheit");
        break;
    case 1130:
        return QString("Pascal");
        break;
    case 1131:
        return QString("Gigapascal");
        break;
    case 1132:
        return QString("Megapascal");
        break;
    case 1133:
        return QString("Kilopascal");
        break;
    case 1137:
        return QString("bar");
        break;
    case 1138:
        return QString("millibar");
        break;
    case 1342:
        return QString("percent");
        break;
    case 1347:
        return QString("cubic meter per second");
        break;
    default:
        return QString("\"" + QString::number(id) + "\"");
        break;
    }
}


int Parser::US16toInt(std::vector<int> in){
    return (int)in.at(0)*pow(2,8) + (int)in.at(1);
}


float Parser::Float101toFloat(std::vector<int> in){
    // regarding page 150: 18 Blocks are going to be received, the last 5 blocks belong to the out (value + status in Data Type 101)
    // regarding to page 52:  Data Type 101 consists of 4 Byte value + 1 byte status
    union conv{
        unsigned char buf[4];
        float number;
    }myconv;
    for(int i = 0; i < 4; i++)
        myconv.buf[i] = in.at(3-i); //bytes seem to be received in reverse order
    return myconv.number;//in.at(val.second-5) + val.first.at(val.second-4) + val.first.at(val.second-3) + val.first.at(val.second-2);
}


QString Parser::Float101Status(int in){
    const int pattern = pow(2,7) + pow(2,6);
    switch(in&pattern)
    {
    case 0:
        return QString("BAD");
        break;
    case (int)pow(2,6):
        return QString("UNCERTAIN");
        break;
    case (int)pow(2,7):
        return QString("GOOD (Non Cascade)");
        break;
    case (pattern):
        return QString("GOOD (Cascade)");
        break;
    default:
        return QString("unknown");
        break;
    }
}


QString Parser::ActMode(int in){
    const int b7 = pow(2,7);
    const int b6 = pow(2,6);
    const int b5 = pow(2,5);
    const int b4 = pow(2,4);
    const int b3 = pow(2,3);
    const int b2 = pow(2,2);
    const int b1 = pow(2,1);
    const int b0 = pow(2,0);

    if (in & b7)
        return QString("Out of Service");
    else if (in & b6)
        return QString("Initialisation Manual");
    else if (in & b5)
        return QString("Local Override");
    else if (in & b4)
        return QString("Manual");
    else if (in & b3)
        return QString("Automatic");
    else if (in & b2)
        return QString("Cascade");
    else if (in & b1)
        return QString("Remote-Cascade");
    else if (in & b5)
        return QString("Remote-Output");
    return QString("unknown");
}


QString Parser::ActAlarm(int in){
    const int b7 = pow(2,7);
//    const int b6 = pow(2,6);
//    const int b5 = pow(2,5);
    const int b4 = pow(2,4);
    const int b3 = pow(2,3);
    const int b2 = pow(2,2);
    const int b1 = pow(2,1);
    const int b0 = pow(2,0);

    QString out = "";
    if (in & b7)
        out.append("Update Event ");
    if (in & b4)
        out.append("LO_Alarm ");
    if (in & b3)
        out.append("LO_LO_Alarm ");
    if (in & b2)
        out.append("HI_HI_Alarm ");
    if (in & b1)
        out.append("HI_Alarm ");
    if (in & b0)
        out.append("Discrete Alarm");
    return out;
}


QString Parser::VisibleStringToString(std::vector<int> in){
    char c[32];
    for (int i = 0; i < 16; i++)
        c[i] = in.at(i);
    return QString(c);
}
