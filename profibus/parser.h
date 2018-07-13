#ifndef PARSER_H
#define PARSER_H
#include "QString"


class Parser
{
public:
    Parser();
    static int US16toInt(std::vector<int> in);
    static QString parseDS36Unit(std::vector<int> in);
    static float Float101toFloat(std::vector<int> in);
};

#endif // PARSER_H
