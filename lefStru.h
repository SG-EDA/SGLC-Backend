#pragma once
#include <QString>
#include <vector>
#include <map>
#include "pos.h"
#include "help.h"
#include <QStringList>
#include <optional>
using namespace std;

class rect
{
public:
    pos p1;
    pos p2;
    rect(pos p1, pos p2):p1(p1),p2(p2){}

    static rect getRect(QString stri, float w, float h)
    {
        QStringList strList=help::splitSpace(stri);
        for(int i=0;i<strList.length();i++)
        {
            float x1,y1,x2,y2;
            if(strList[i]=="RECT")
            {
                x1=strList[i+1].toFloat();
                y1=strList[i+2].toFloat();
                x2=strList[i+3].toFloat();
                y2=strList[i+4].toFloat();
                pos p1(x1,y1,w,h); //左下原点
                pos p2(x2,y2,w,h);
                rect r(p1,p2);
                return r;
            }
        }
    }

    void setToLayout(float setX, float setY, QString dire)
    {
        this->p1.setDire(dire);
        this->p1.setToLayout(setX,setY);
        this->p2.setDire(dire);
        this->p2.setToLayout(setX,setY);
    }

    bool isIntersect(const rect& r)
    {
        //检测两矩形是否相交
    }
};

class line
{
public:
    float x1;
    float y1;
    float x2;
    float y2;
    QString metal;

    rect toRect()
    {
        return rect(pos(x1,y1),pos(x2,y2));
    }
};


namespace LEF
{

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

    optional<rect> checkOBS(line l)
    {
        if (this->o.find(l.metal) == this->o.end())
            return optional<rect>(); //这一层不存在obs
        else
        {
            vector<rect> &allRect=this->o[l.metal];
            for(rect r : allRect)
            {
                if(r.isIntersect(l.toRect()))
                    return r;
            }
            return optional<rect>(); //找不到相交的obs
        }
    }
};

struct via
{
    QString name;
    float spacing;
};

}
