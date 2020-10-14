#pragma once

class pos
{
private:
    float w;
    float h;
    bool isOrigin; //中心原点

public:
    float x;
    float y;
    pos(float x,float y,float w,float h,bool isOrigin=false):
        x(x),y(y),w(w),h(h),isOrigin(isOrigin){}

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
    void setToCell(float setX, float setY)
    {

    }
    bool getIsOrigin() { return this->isOrigin; }
};
