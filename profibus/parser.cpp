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
    default:
        return QString("lightyears");
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
