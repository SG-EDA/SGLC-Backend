#pragma once

class pos
{
    float x;
    float y;
    pos(float x,float y):x(x),y(y){};

    pos toS();
    pos toW();
    pos toE();
    pos toFN();
    pos toFS();
    pos toFW();
    pos toFE();
};
