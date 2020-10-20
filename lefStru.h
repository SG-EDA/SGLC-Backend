#pragma once
#include <QString>
#include <vector>
#include <map>
#include "pos.h"
#include "help.h"
#include <QStringList>
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
        {
            r.p1.setDire(dire);
            r.p1.setToLayout(setX,setY);
            r.p2.setDire(dire);
            r.p2.setToLayout(setX,setY);
        }
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
    }
};

struct via
{
    QString name;
    float spacing;
};

}
