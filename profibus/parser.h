#ifndef PARSER_H
#define PARSER_H
#include "QString"
#include "vector"
#include "cmath"



class Parser
{
public:
    Parser();
    static int US16toInt(std::vector<int> in);
    static QString parseDS36Unit(std::vector<int> in);
    static float Float101toFloat(std::vector<int> in);
    static QString parseUnitId(int id);
    static QString Float101Status(int in);
    static QString ActMode(int in);
    static QString ActAlarm(int in);
    static QString VisibleStringToString(std::vector<int> in);
};

#endif // PARSER_H
