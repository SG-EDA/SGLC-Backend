#pragma once
#include <QString>

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
    pos(float x,float y,float w,float h,bool isOrigin=false):
        x(x),y(y),w(w),h(h),isOrigin(isOrigin){}

    void setDire(QString dire)
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
        this->inLayout=false;
    }

    bool getIsOrigin() { return this->isOrigin; }
};
