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
    rect() : p1(pos(0,0)), p2(pos(0,0)) {} //给tie使用的洞

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

    tuple<float,float> getMidPos() const
    {
        float r2_x_mid=(this->p1.x+this->p2.x)/2;
        float r2_y_mid=(this->p2.y+this->p2.y)/2;
        return make_tuple(r2_x_mid,r2_y_mid);
    }

    bool isIntersect(const rect& r)
    {
        float r1_x_mid,r1_y_mid,r2_x_mid,r2_y_mid;
        tie(r1_x_mid,r1_y_mid)=r.getMidPos();
        tie(r2_x_mid,r2_y_mid)=this->getMidPos();

        float r1Width=r.p2.x-r.p1.x;
        float r1Height=r.p2.y-r.p1.y;
        float r2Width=this->p2.x-this->p1.x;
        float r2Height=this->p2.y-this->p1.y;

        if (
            abs(r1_x_mid - r2_x_mid) < r1Width / 2.0 + r2Width / 2.0 //横向判断
            &&
            abs(r1_y_mid - r2_y_mid) < r1Height / 2.0 + r2Height / 2.0 //纵向判断
            )
        {
            return true; //碰撞
        }
        else
            return false;
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
    int m1;
    int m2;
    float spacing;
};

struct metal
{
    int m;
    float minWidth;
    float spacing;
    float area=-1; //-1为无约束
    bool vertical; //false为horizontal
};

}
