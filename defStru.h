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
        bool leftBrack=false;
        bool oneNum=false;
        bool rightBrack=false;

        for(QString i : strList)
        {
            if(leftBrack==false)
            {
                if(i.indexOf("inst"))
                    com.instName=i;
                else if(i.indexOf("CELL"))
                    com.cellName=i;
                else if(i=="(") //进入左括号状态
                    leftBrack=true;
            }
            else //左括号状态
            {
                if(rightBrack==false)
                {
                    if(help::VerifyNumber(i))
                    {
                        if(oneNum==false)
                        {
                            com.x=i.toFloat();
                            oneNum=true;
                        }
                        else
                        {
                            com.y=i.toFloat();
                        }
                    }
                    else
                    {
                        if(i==")") //进入找方向状态
                            rightBrack=true;
                    }
                }
                else //找方向
                {
                    com.dire=i;
                    break;
                }
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
