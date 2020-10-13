#pragma once
#include <QString>
#include <vector>
#include <QStringList>
using namespace std;

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

        stri=stri.replace("  "," ");
        QStringList strList=stri.split(" ");

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
                    if(VerifyNumber(i))
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

private:
    static bool VerifyNumber(QString str)
    {
        std::string temp = str.toStdString();
        for (int i = 0; i < str.length(); i++)
        {
            if (temp[i]<'0' || temp[i]>'9')
                return false;
        }
        return true;
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
