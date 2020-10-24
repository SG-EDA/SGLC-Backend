#pragma once
#include "lefParser.h"
#include "defParser.h"

class layout
{
private:
    optional<rect> checkLine(line l)
    {
        for(LEF::cell &c : this->allCell) //fix:可以尝试只检测近邻的以剪枝
        {
            auto result=c.checkOBS(l);
            if(result.has_value())
                return result;
        }

        rect lr=l.toRect();
        for(line &li : this->allLine) //fix:剪枝同上
        {
            if(l.metal==li.metal)
            {
                rect lir=li.toRect();
                if(lr.isIntersect(lir))
                    return lir;
            }
        }

        return optional<rect>();
    }

    line connect(LEF::pin &p1, LEF::pin &p2)
    {
        //避开目前有线位置和obs连接两个pin
        //1.查找二者最近的两个rect
        //2.尝试横/竖走线，检测每条横/竖线中无法走线（与障碍矩形相交），此时要获取到整个障碍矩形 - 使用checkLine
        //3.在此处停止，进行局部修正（绕过障碍矩形/走到障碍矩形的一个距目标rect最近的顶点）
        //4.使用修正后的坐标继续横/竖走线（循环）
    }

    LEF::pin& findLEFPin(QString instName, QString pinName)
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

    static float getPinDist(LEF::pin &p1, LEF::pin &p2)
    {
        //查找两个pin中最近rect的距离
    }

    vector<LEF::pin> toLEFPinVec(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin> result;
        for(auto i : allPin)
            result.push_back(this->findLEFPin(i.instName,i.pinName));
        return result;
    }

    vector<LEF::pin> sortAllPin(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin> LEFallPin=this->toLEFPinVec(allPin);
        //fix:对LEFallPin进行排序
        return LEFallPin;
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
            auto LEFallPin=this->sortAllPin(n.allPin);
            for(int i=1;i<LEFallPin.size();i++)
            {
                line l=this->connect(LEFallPin[i-1],LEFallPin[i]);
                this->allLine.push_back(l);
            }
        }
    }
};

