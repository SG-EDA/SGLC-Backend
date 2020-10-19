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
private:
    line connect(LEF::pin &p1, LEF::pin &p2) //fix:还需要支持多个
    {
        //避开目前有线位置和obs连接两个pin
    }

    LEF::pin& findPinPos(QString instName, QString pinName)
    {
        for(LEF::cell &c : this->allCell)
        {
            if(c.instName==instName)
            {
                for(LEF::pin &p : c.allPin)
                {
                    if(p.name==pinName)
                        return p;
                }
            }
        }
    }

public:
    defParser dp;
    lefParser lp;
    vector<line> allLine; //将net转化为line
    vector<LEF::cell> allCell; //版图中放置的所有cell（转换为版图坐标系）

    layout(defParser dp, lefParser lp) : dp(dp), lp(lp)
    {
        for(DEF::component &c : dp.allComponent)
        {
            LEF::cell lefC=lp.getCell(c.cellName);
            lefC.instName=c.instName;
            lefC.setToLayout(c.x,c.y,c.dire);
            this->allCell.push_back(lefC);
        }
    }

    void connectAllNet()
    {
        for(DEF::net &n : dp.allNet)
        {
            for(DEF::pin &p : n.allPin)
            {
                //找出里面的两个（多个）pin，然后调用this.connect得到line
            }
        }
    }
};

