#pragma once
#include "lefStru.h"
#include "help.h"

class lefParser
{
private:
    int dbu=2000;
    qstringList codeList;

    int getPin(int i, LEF::cell &c)
    {
        LEF::pin p;
        //get pinName
        p.name=help::getLastElm(this->codeList[i],"PIN");
        i++;
        //实际读内容
        for(int j=i;j<this->codeList.size();j++)
        {
            qstring stri=this->codeList[j];
            if(stri.find("LAYER"))
            {
                qstring layer=help::getLastElm(stri,"LAYER");
                int m=qstring(layer[layer.size()-1]).toInt();
                p.metal=this->getMetal(m);
            }
            else if(stri.find("RECT "))
            {
                rect r=rect::getRect(stri,c.sizeA1,c.sizeA2);
                r.plusDbu(dbu);
                p.allRect.push_back(pinRect(r));
            }
            else if(stri.find("END "+p.name))
            {
                c.allPin.push_back(p);
                return j;
            }
        }
        return -1;
    }

    int getOBS(int i, LEF::cell &c)
    {
        qstring layerName;
        for(int j=i;i<this->codeList.size();j++)
        {
            qstring stri=this->codeList[j];
            if(stri.find("LAYER"))
                layerName=help::getLastElm(stri,"LAYER");
            else
            {
                if(layerName!="")
                {
                    if(stri.find("RECT"))
                    {
                        rect r=rect::getRect(stri,c.sizeA1,c.sizeA2);
                        r.plusDbu(dbu);
                        c.o[layerName].push_back(r);
                    }
                }
                if(stri.find("END"))
                    return j;
            }
        }
        return -1;
    }

    LEF::metal _getMetal(int _m)
    {
        qstring metalName="METAL"+qstring::number(_m);
        LEF::metal m;
        bool findMet=false;
        for(int i=0;i<codeList.size();i++)
        {
            qstring stri=codeList[i];
            if(findMet==false && stri=="LAYER "+metalName) //找到metal位置，进状态
            {
                m.ID=_m;
                findMet=true;
            }
            else if(findMet)
            {
                if(stri.find("SPACING"))
                {
                    m.spacing=help::getLastElm(stri,"SPACING").toFloat();
                    m.spacing*=dbu;
                }
                else if(stri.find("MINWIDTH"))
                    continue;
                else if(stri.find("WIDTH"))
                {
                    m.minWidth=help::getLastElm(stri,"WIDTH").toFloat();
                    m.minWidth*=dbu;
                }
                else if(stri.find("AREA"))
                {
                    m.area=help::getLastElm(stri,"AREA").toFloat();
                    m.area*=dbu;
                }
                else if(stri.find("DIRECTION"))
                {
                    if(stri.find("VERTICAL"))
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

    lefParser(qstring code, int minMetalID, int maxMetalID) : minMetalID(minMetalID)
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
        qstring viaName="via"+qstring::number(m1);
        qstring m1Name="METAL"+qstring::number(m1);
        qstring m2Name="METAL"+qstring::number(m1+1);
        LEF::via v;
        bool findVia=false;
        for(int i=0;i<codeList.size();i++)
        {
            qstring stri=codeList[i];
            if(findVia==false && stri=="VIA "+viaName+" DEFAULT") //找到via位置，进状态
            {
                v.m1=m1;
                findVia=true;
            }
            else if(findVia)
            {
                if(stri.find("LAYER VIA"+qstring::number(m1)))
                {
                    v.viaRect=rect::getRect(codeList[i+1],-1,-1,true);
                    v.viaRect.plusDbu(dbu);
                }
                else if(stri.find("LAYER "+m1Name))
                {
                    v.m1Rect=rect::getRect(codeList[i+1],-1,-1,true);
                    v.m1Rect.plusDbu(dbu);
                }
                else if(stri.find("LAYER "+m2Name))
                {
                    v.m2Rect=rect::getRect(codeList[i+1],-1,-1,true);
                    v.m2Rect.plusDbu(dbu);
                }
                else if(stri.find("END "+viaName))
                    break;
            }
        }
        return v;
    }

    LEF::cell getCell(qstring cellName)
    {
        LEF::cell c;
        bool findCell=false;
        for(int i=0;i<codeList.size();i++)
        {
            qstring stri=codeList[i];
            if(findCell==false && stri=="MACRO "+cellName) //找到CELL位置，进状态
            {
                c.cellName=cellName;
                findCell=true;
            }
            else if(findCell)
            {
                if(stri.find("SIZE"))
                {
                    qstringList sizeList=help::splitSpace(stri);
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
                else if(stri.find("PIN"))
                    i=this->getPin(i,c);
                else if(stri.find("OBS"))
                    i=this->getOBS(i+1,c);
                else if(stri.find("END "+cellName))
                    break;
            }
        }
        c.plusDbu(dbu);
        return c;
    }
};
