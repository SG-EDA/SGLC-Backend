#pragma once
#include "lefStru.h"
#include "help.h"
#include <QStringList>

class lefParser
{
private:
    QStringList codeList;

    int getPin(int i, LEF::cell &c)
    {
        LEF::pin p;
        //get pinName
        p.name=help::getLastElm(this->codeList[i],"PIN");
        i++;
        //实际读内容
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri.indexOf("LAYER")!=-1)
            {
                QString layer=help::getLastElm(stri,"LAYER");
                int m=QString(layer[layer.size()-1]).toInt();
                p.metal=this->getMetal(m);
            }
            else if(stri.indexOf("RECT ")!=-1)
            {
                rect r=rect::getRect(stri,c.sizeA1,c.sizeA2);
                p.allRect.push_back(pinRect(r));

            }
            else if(stri.indexOf("END "+p.name)!=-1)
            {
                c.allPin.push_back(p);
                return j;
            }
        }
        return -1;
    }

    int getOBS(int i, LEF::cell &c)
    {
        QString layerName;
        for(int j=i;i<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri.indexOf("LAYER")!=-1)
                layerName=help::getLastElm(stri,"LAYER");
            else
            {
                if(layerName!="")
                {
                    if(stri.indexOf("RECT")!=-1)
                    {
                        rect r=rect::getRect(stri,c.sizeA1,c.sizeA2);
                        c.o[layerName].push_back(r);
                    }
                }
                if(stri.indexOf("END")!=-1)
                    return j;
            }
        }
        return -1;
    }

    LEF::metal _getMetal(int _m)
    {
        QString metalName="METAL"+QString::number(_m);
        LEF::metal m;
        bool findMet=false;
        for(int i=0;i<codeList.length();i++)
        {
            QString stri=codeList[i];
            if(findMet==false && stri=="LAYER "+metalName) //找到metal位置，进状态
            {
                m.ID=_m;
                findMet=true;
            }
            else if(findMet)
            {
                if(stri.indexOf("SPACING")!=-1)
                    m.spacing=help::getLastElm(stri,"SPACING").toFloat();
                else if(stri.indexOf("MINWIDTH")!=-1)
                    continue;
                else if(stri.indexOf("WIDTH")!=-1)
                    m.minWidth=help::getLastElm(stri,"WIDTH").toFloat();
                else if(stri.indexOf("AREA")!=-1)
                    m.area=help::getLastElm(stri,"AREA").toFloat();
                else if(stri.indexOf("DIRECTION")!=-1)
                {
                    if(stri.indexOf("VERTICAL")!=-1)
                        m.vertical=true;
                    else
                        m.vertical=false;
                }
                else if(stri=="END "+metalName)
                    break;
            }
        }
        return m;
    }

    const int minMetalID;

public:
    vector<LEF::metal> allMetal;

    lefParser(QString code, int minMetalID, int maxMetalID) : minMetalID(minMetalID)
    {
        codeList=code.split("\n");

        for(int i=minMetalID;i<=maxMetalID;i++)
            this->allMetal.push_back(this->_getMetal(i));
    }

    LEF::metal getMetal(int ID)
    {
        return this->allMetal[ID-minMetalID];
    }

    LEF::via getVia(int m1)
    {
        QString viaName="via"+QString::number(m1);
        QString m1Name="METAL"+QString::number(m1);
        QString m2Name="METAL"+QString::number(m1+1);
        LEF::via v;
        bool findVia=false;
        for(int i=0;i<codeList.length();i++)
        {
            QString stri=codeList[i];
            if(findVia==false && stri=="VIA "+viaName+" DEFAULT") //找到via位置，进状态
            {
                v.m1=m1;
                findVia=true;
            }
            else if(findVia)
            {
                if(stri.indexOf("LAYER "+viaName)!=-1)
                    v.viaRect=rect::getRect(codeList[i+1],-1,-1,true);
                else if(stri.indexOf("LAYER "+m1Name)!=-1)
                    v.viaRect=rect::getRect(codeList[i+1],-1,-1,true);
                else if(stri.indexOf("LAYER "+m2Name)!=-1)
                    v.viaRect=rect::getRect(codeList[i+1],-1,-1,true);
                else if(stri=="END "+viaName)
                    break;
            }
        }
        return v;
    }

    LEF::cell getCell(QString cellName)
    {
        LEF::cell c;
        bool findCell=false;
        for(int i=0;i<codeList.length();i++)
        {
            QString stri=codeList[i];
            if(findCell==false && stri=="MACRO "+cellName) //找到CELL位置，进状态
            {
                c.cellName=cellName;
                findCell=true;
            }
            else if(findCell)
            {
                if(stri.indexOf("SIZE")!=-1)
                {
                    QStringList sizeList=help::splitSpace(stri);
                    for(int j=0;j<sizeList.size();j++)
                    {
                        if(sizeList[j]=="SIZE")
                        {
                            c.sizeA1=sizeList[j+1].toFloat();
                            c.sizeA2=sizeList[j+3].toFloat();
                            break;
                        }
                    }
                }
                else if(stri.indexOf("PIN")!=-1)
                    i=this->getPin(i,c);
                else if(stri.indexOf("OBS")!=-1)
                    i=this->getOBS(i+1,c);
                else if(stri=="END "+cellName)
                    break;
            }
        }
        return c;
    }
};
