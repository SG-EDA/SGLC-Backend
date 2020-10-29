#pragma once
#include <vector>
#include <map>
#include <optional>
#include "rect.h"
using namespace std;

namespace LEF
{

struct via
{
    int m1;
    int m2;
    float spacing;
};

struct metal
{
    int m;
    float width;
    float spacing;
    float area=-1; //-1为无约束
    bool vertical; //false为horizontal

    QString getName() { return "METAL"+QString::number(m); }
    bool operator==(const metal &m) { return this->m==m.m; }
};

struct pin
{
    QString name;
    QString layer;
    vector<rect> allRect;

    void setToLayout(float setX, float setY, QString dire)
    {
        for(rect &r : this->allRect)
            r.setToLayout(setX,setY,dire);
    }
};

typedef map<QString,vector<rect>> obs; //金属,obs块

struct cell
{
    QString cellName;
    QString instName; //用于在layout中反查使用
    float sizeA1;
    float sizeA2;
    vector<pin> allPin;
    obs o;

    void setToLayout(float setX, float setY, QString dire)
    {
        for(pin &p : this->allPin)
            p.setToLayout(setX,setY,dire);
        for(auto &p : this->o)
        {
            vector<rect> &allRect=p.second;
            for(rect &r : allRect)
                r.setToLayout(setX,setY,dire);
        }
    }
};

}


class line
{
public:
    float x1;
    float y1;
    float x2;
    float y2;
    LEF::metal metal;

    rect toRect()
    {
        return rect(pos(x1,y1),pos(x2,y2));
    }

    optional<rect> checkOBS(LEF::cell &c)
    {
        QString metalName=this->metal.getName();
        rect result;

        auto checkAndUpdate=[&result,this](vector<rect> allRect)
        {
            for(rect r : allRect)
            {
                if(r.isIntersect(this->toRect(),this->metal.spacing,this->metal.width))
                {
                    if(r.isLowerLeft(result))
                        result=r;
                }
            }
        };

        //检查pin里的rect
        for(LEF::pin &p : c.allPin)
        {
            if(p.layer==metalName)
                checkAndUpdate(p.allRect);
        }

        //检查obs
        if (!(c.o.find(metalName) == c.o.end())) //这一层存在obs
        {
            vector<rect> &allRect=c.o[metalName];
            checkAndUpdate(allRect);
        }

        if(result.isNull())
            return optional<rect>(); //找不到相交的
        else
            return result;
    }
};
