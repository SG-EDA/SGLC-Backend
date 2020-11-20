#pragma once
#include "QString.h"
#include <vector>
#include "help.h"
using namespace std;

namespace DEF
{

struct component
{
    qstring instName;
    qstring cellName;
    float x;
    float y;
    qstring dire;

    static component get(qstring stri)
    {
        component com;
        qstringList strList=help::splitSpace(stri);

        for(int i=0;i<strList.size();i++)
        {
            if(strList[i]=="-")
            {
                com.instName=strList[i+1];
                com.cellName=strList[i+2];
            }
            else if(strList[i]=="(")
            {
                com.x=strList[i+1].toFloat();
                com.y=strList[i+2].toFloat();
            }
            else if(strList[i]==")")
            {
                com.dire=strList[i+1];
                break;
            }
        }
        return com;
    }
};

struct pin
{
    qstring instName;
    qstring pinName;
};

struct net
{
    qstring name;
    vector<pin> allPin; //连接这些pin

    net(qstring name):name(name){}
};

}
