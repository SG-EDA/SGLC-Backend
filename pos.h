#pragma once

class pos
{
    float x;
    float y;
    pos(float x,float y):x(x),y(y){}

    pos toS(float w, float h);
    pos toW(float w, float h);
    pos toE(float w, float h);
    pos toFN(float w, float h);
    pos toFS(float w, float h);
    pos toFW(float w, float h);
    pos toFE(float w, float h);
};
