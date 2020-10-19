#pragma once
#include "lefParser.h"
#include "defParser.h"

class line
{
public:
    float x1;
    float y1;
    float x2;
    float y2;
};

class layout
{
public:
    defParser dp;
    lefParser lp;
    vector<line> allLine; //将net转化为line
    layout(defParser dp, lefParser lp) : dp(dp), lp(lp) {}
};

