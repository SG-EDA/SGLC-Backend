#pragma once
#include <QString>
#include <vector>
#include <QStringList>
#include "help.h"
using namespace std;

namespace DEF
{

struct component
{
    QString instName;
    QString cellName;
    float x;
    float y;
    QString dire;

    static component get(QString stri)
    {
        component com;
        QStringList strList=help::splitSpace(stri);

        for(int i=0;i<strList.length();i++)
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
    QString comName;
    QString pinName;
};

struct net
{
    QString name;
    vector<pin> allPin;

    net(QString name):name(name){}
};

}
