#pragma once
#include "QString.h"

class pos
{
private:
    float w;
    float h;
    bool isOrigin; //中心原点
    bool inLayout=false;

    pos toS()
    {
        if(this->isOrigin==true)
            return pos(-this->x,-this->y,this->w,this->h);
        else
            return pos(this->w-this->x,this->h-this->y,this->w,this->h);
    }
    pos toW()
    {
        if(this->isOrigin==true)
            return pos(-this->x,this->y,this->w,this->h);
        else
            return pos(this->h-this->y,this->w-this->x,this->h,this->w);
    }
    pos toE()
    {
        if(this->isOrigin==true)
            return pos(this->x,-this->y,this->w,this->h);
        else
            return pos(this->y,this->w-this->x,this->h,this->w);
    }
    pos toFN()
    {
        if(this->isOrigin==true)
            return pos(-this->x,this->y,this->w,this->h);
        else
            return pos(this->w-this->x,this->y,this->w,this->h);
    }
    pos toFS()
    {
        if(this->isOrigin==true)
            return pos(this->x,-this->y,this->w,this->h);
        else
            return pos(this->x,this->w-this->y,this->w,this->h);
    }
    pos toFW()
    {
        if(this->isOrigin==true)
            return pos(this->y,this->x,this->h,this->w);
        else
            return pos(this->y,this->x,this->h,this->w);
    }
    pos toFE()
    {
        if(this->isOrigin==true)
            return pos(-this->y,-this->x,this->h,this->w);
        else
            return pos(this->h-this->y,this->w-this->x,this->h,this->w);
    }

public:
    float x;
    float y;
    pos(float x,float y,float w,float h,bool isOrigin=false): //如果不做翻转，w和h可以不要（随便传个-1就行）
        x(x),y(y),w(w),h(h),isOrigin(isOrigin){}
    pos(float x,float y) : x(x), y(y), isOrigin(false), inLayout(true) {}
    pos(){} //洞

    void setDire(qstring dire)
    {
        if(dire=="S")
            this->toS();
        else if(dire=="E")
            this->toE();
        else if(dire=="W")
            this->toW();
        else if(dire=="FN")
            this->toFN();
        else if(dire=="FE")
            this->toFE();
        else if(dire=="FS")
            this->toFS();
        else if(dire=="FW")
            this->toFW();
    }

    void setToLayout(float setX, float setY)
    {
        this->x+=setX;
        this->y+=setY;
        this->isOrigin=false;
        this->inLayout=true;
    }

    bool getIsOrigin() { return this->isOrigin; }
    bool getInLayout() { return this->inLayout; }

    qstring toStr()
    {
        return "( "+qstring::number(int(this->x))+" "+qstring::number(int(this->y))+" )";
    }
};
