#pragma once
#include "defStru.h"
#include <QStringList>

class parser
{
private:
    QStringList codeList;

    int parseComponents(int i)
    {
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[i];
            if(stri.indexOf("inst")!=-1)
                this->allComponent.push_back(component::get(stri));
            else if(stri=="END COMPONENTS")
                return j;
        }
        return -1;
    }

    int parsePins(int i)
    {

    }

    int getPin(int i, net &n)
    {
        for(int j=i;j<this->codeList.length();j++)
        {
            QString stri=this->codeList[i];
            if(stri.indexOf(";"))
                return j;
            else
            {
                //拆括号
                for(int i=0;i<stri.length();i++)
                {
                    if(stri[i]=='(') //进入单个括号状态
                    {
                        i++; //前进到左括号后
                        bool isMeetFirst=false; //识别到pinname
                        bool isSpace=false; //有没有遇到中间空格
                        bool finished=false;
                        pin p;
                        for(;stri[i]!=')';i++)
                        {
                            auto c=stri[i];
                            if(isSpace==false) //还在找pinname
                            {
                                if(isMeetFirst==false) //还在前面空格
                                {
                                    if(c!=' ') //遇到非空格出状态
                                        isMeetFirst=false;
                                    continue;
                                }
                                else //正在查pinname
                                {
                                    if(c==' ') //遇到空格出状态
                                        isSpace=false;
                                    else
                                        p.pinName+=QString(c);
                                }
                            }
                            else //找cellname
                            {
                                if(c==')') //出状态，准备进下个括号
                                {
                                    i++; //准备进行下一个括号
                                    n.allPin.push_back(p);
                                    break; //跳出循环
                                }
                                else if(c==' ') //出状态，找反括号
                                    finished=true;
                                else if(finished==false) //正常字符
                                    p.comName+=QString(c);
                            }
                        }
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
            QString stri=this->codeList[i];
            if(stri.indexOf("net")!=-1)
            {
                net n(getNetName(stri));
                i=this->getPin(i,n);
                this->allNet.push_back(n);
            }
            else if(stri=="END NETS")
                return j;
        }
        return -1;
    }

public:
    vector<component> allComponent;
    vector<pin> allPin;
    vector<net> allNet;

    parser(QString code)
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
