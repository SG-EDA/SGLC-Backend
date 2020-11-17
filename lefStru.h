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
    rect viaRect;
    rect m1Rect;
    rect m2Rect;
};

struct metal
{
    int ID;
    float minWidth;
    const float maxNeedWidth=0.6;
    float spacing;
    float area=-1; //-1为无约束
    bool vertical; //false为horizontal

    QString getName() { return "METAL"+QString::number(ID); }
    bool operator==(const metal &m) { return this->ID==m.ID; }
};

struct pin
{
    QString name;
    LEF::metal metal;
    vector<pinRect> allRect;

    void setToLayout(float setX, float setY, QString dire)
    {
        for(pinRect &r : this->allRect)
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
    float endPosX;
    float endPosY;
    float width;

    line(){}
    line(float x1,float y1,float x2,float y2,LEF::metal metal, float endPosX, float endPosY,float width=-1) :
        x1(x1), y1(y1), x2(x2), y2(y2), metal(metal), endPosX(endPosX), endPosY(endPosY)
    {
        if(width==-1)
            this->width=metal.minWidth;
        else
            this->width=width;
    }

    line(float x,float y,float y2,LEF::metal metal,float endPosX, float endPosY,bool isVertical=true,float width=-1) :
        metal(metal), endPosX(endPosX), endPosY(endPosY)
    {
        if(width==-1)
            this->width=metal.minWidth;
        else
            this->width=width;

        float w=this->width/2;
        if(isVertical)
        {
            this->x1=x-w; //x是左边的
            this->x2=x+w;
            this->y1=y;
            this->y2=y2;
        }
        else //x y反转
        {
            this->y1=x-w; //x是下边的
            this->y2=x+w;
            this->x1=y;
            this->x2=y2;
        }
    }

    rect toRect()
    {
        return rect(pos(x1,y1),pos(x2,y2));
    }

    optional<rect> checkOBS(LEF::cell &c)
    {
        QString metalName=this->metal.getName();
        rect result;

        //检查pin里的rect
        for(LEF::pin &p : c.allPin)
        {
            if(p.metal==this->metal)
            {
                for(pinRect r : p.allRect)
                {
                    if(r.isIntersect(this->toRect(),this->metal.spacing,this->width))
                    {
                        if(r.isLowerLeft(result))
                            result=r;
                    }
                }
            }
        }

        //检查obs
        if (!(c.o.find(metalName) == c.o.end())) //这一层存在obs
        {
            vector<rect> &allRect=c.o[metalName];
            for(rect r : allRect)
            {
                if(r.isIntersect(this->toRect(),this->metal.spacing,this->width))
                {
                    if(r.isLowerLeft(result))
                        result=r;
                }
            }
        }

        if(result.isNull())
            return optional<rect>(); //找不到相交的
        else
            return result;
    }

    /*QString getPos1()
    {
        return "( "+QString::number(int(this->x1))+" "+QString::number(int(this->y1))+" )";
    }

    QString getPos2()
    {
        return "( "+QString::number(int(this->x2))+" "+QString::number(int(this->y2))+" )";
    }*/
	
    tuple<float,float> getCrossCenter(line &l)
    {
        float x,y;
        //计算中心坐标
        float this_center_x = (this->x1 + this->x2)/2;
        float this_center_y = (this->y1 + this->y2)/2;
        float l_center_x    = (l.x1     + l.x2    )/2;
        float l_center_y    = (l.y1     + l.y2    )/2;
        //中心的相对位置
        float location_x = this_center_x - l_center_x;
        float location_y = this_center_y - l_center_y;
        //根据相对位置判断相交区域
        if((location_x > 0) && (location_y > 0))    //this在右上
        {
            x = (l.x2 + this->x1)/2;
            y = (l.y2 + this->y1)/2;
        }
        else if((location_x < 0) && (location_y > 0))   //在左上
        {
            x = (l.x1 + this->x2)/2;
            y = (l.y2 + this->y1)/2;
        }
        else if((location_x > 0) && (location_y < 0))   //在右下
        {
            x = (l.x2 + this->x1)/2;
            y = (l.y1 + this->y2)/2;
        }
        else   //在左下
        {
            x = (l.x1 + this->x2)/2;
            y = (l.y1 + this->y2)/2;
        }

        return make_tuple(x,y);
    }

    tuple<pos,pos> getMidLine()
    {
        float dx=this->x2-this->x1;
        float dy=this->y2-this->y1;
        if(dx<dy)
        {
            float xMid=(x2-x1)/2;
            return make_tuple(pos(xMid,this->y1),pos(xMid,this->y2));
        }
        else
        {
            float yMid=(y2-y1)/2;
            return make_tuple(pos(this->x1,yMid),pos(this->x2,yMid));
        }
    }
};
