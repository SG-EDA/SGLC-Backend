#pragma once

class pos
{
private:
    float w;
    float h;
    bool isOrigin;

public:
    float x;
    float y;
    pos(float x,float y,float w,float h):
        x(x),y(y),w(w),h(h){}

    pos toS();
    pos toW();
    pos toE();
    pos toFN();
    pos toFS();
    pos toFW();
    pos toFE();
    void setToCell(float cellX, float cellY)
    {
        this->x+=cellX;
        this->y+=cellY;
        this->isOrigin=true;
    }
    bool getIsOrigin() { return this->isOrigin; }
};
