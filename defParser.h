#pragma once
#include "defStru.h"
#include <QStringList>

class defParser
{
private:
    QStringList codeList;

    int parseComponents(int i)
    {
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri.indexOf("inst")!=-1)
                this->allComponent.push_back(DEF::component::get(stri));
            else if(stri=="END COMPONENTS")
                return j;
        }
        return -1;
    }

    int parsePins(int i)
    {
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri=="END PINS")
                return j;
        }
    }

    int getPin(int i, DEF::net &n)
    {
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri.indexOf(";")!=-1)
                return j;
            else
            {
                QStringList strList=help::splitSpace(stri);
                for(int i=0;i<strList.length();i++)
                {
                    if(strList[i]=="(")
                    {
                        DEF::pin p;
                        p.comName=strList[i+1];
                        p.pinName=strList[i+2];
                        n.allPin.push_back(p);
                    }
                }
            }
        }
        return -1;
    }

    int parseNets(int i)
    {
        auto getNetName=[](QString stri) {
            stri=stri.replace(" ","");
            return stri.mid(1); //截掉-
        };

        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[j];
            if(stri.indexOf("net")!=-1)
            {
                DEF::net n(getNetName(stri));
                j=this->getPin(j,n);
                this->allNet.push_back(n);
            }
            else if(stri=="END NETS")
                return j;
        }
        return -1;
    }

public:
    vector<DEF::component> allComponent;
    vector<DEF::pin> allPin;
    vector<DEF::net> allNet;

    defParser(QString code)
    {
        this->codeList=code.split("\n");
        for(int i=0;i<this->codeList.length();i++)
        {
            QString stri=this->codeList[i];
            if(stri.indexOf("COMPONENTS")!=-1)
                i=this->parseComponents(i+1);
            else if(stri.indexOf("PINS")!=-1)
                i=this->parsePins(i+1);
            else if(stri.indexOf("NETS")!=-1)
                i=this->parseNets(i+1);
            else if(stri=="END DESIGN")
                break;
        }
    }
};
