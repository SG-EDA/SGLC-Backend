#pragma once
#include "router.h"

class codegen
{
private:
    //处理一个net，在后面添加line和via。返回处理完的行下标
    int genNet(int i,qstring &result,vector<line> &allLine, vector<via> &allVia)
    {
        result+=this->l.dp.codeList[i]+"\n"; //把net那行加进去
        i++;

        for(;i<this->l.dp.codeList.size();i++)
        {
            qstring stri=this->l.dp.codeList[i];
            result+=stri+"\n";
            //向下探测有没有到结尾
            qstring stri2=this->l.dp.codeList[i+1];
            if(stri2.find(";")) //fix:目前sample里暂时没有分号非单独一行的
            {
                //到结尾了
                if(!allLine.empty())
                {
                    result+="+ ROUTED ";
                    bool first=true;
                    //把allVia加上
                    for(via v : allVia)
                    {
                        if(!first)
                            result+="NEW ";
                        else
                            first=false;

                        result+=v.m.getName()+" "+v.getPos()+" "+v.getName()+"\n";
                    }
                    //把allLine加上
                    for(line l : allLine)
                    {
                        if(!first)
                            result+="NEW ";
                        else
                            first=false;

                        pos p1,p2;
                        tie(p1,p2)=l.getMidLine();

                        result+=l.metal.getName()+" "+p1.toStr()+" "+p2.toStr()+"\n";
                    }
                }
                break;
            }
        }
        result+=";\n";
        return i+1; //跳过分号那行到下一个net
    }

public:
    router l;
    codegen(router l) : l(l) {}

    qstring doGen()
    {
        qstring result;
        for(int i=0;i<this->l.dp.codeList.size();i++)
        {
            qstring stri=this->l.dp.codeList[i];
            if(stri.find("DESIGN "))
                result+="DESIGN result ;\n";
            else if(stri.find("NETS"))
                result+=this->genNETS(i);
            else
                result+=stri+"\n";
        }
        return result;
    }

    /*QString genNONDEFAULTRULES()
    {
        QString result="NONDEFAULTRULES 1 ;\n";
        result+="- DEFAULT_METAL1_580\n";
        for(LEF::metal &m : l.lp.allMetal)
        {
            result+="+ LAYER "+m.getName()+"\n";
            result+="WIDTH "+QString::number(int(m.width));
        }
        result+=";\n";
        result+="END NONDEFAULTRULES\n";
        return result;
    }*/

    qstring genNETS(int &i)
    {
        qstring result;
        //使用类似NETparser，每个net结束之后添加对应下标的line
        bool findNet=false;
        int netNum=0;
        for(;i<this->l.dp.codeList.size();i++)
        {
            qstring stri=this->l.dp.codeList[i];
            if(!findNet && stri.find("NETS"))
            {
                result+=stri+"\n";
                findNet=true;
            }

            if(findNet)
            {
                if(stri.find("net")) //如果碰到一个net，转到genNet里去把这个处理完
                {
                    i=this->genNet(i,result,l.allNetLine[netNum],l.allNetVia[netNum]);
                    netNum++;
                }

                if(stri.find("END NETS"))
                {
                    //END这句不会在前面添加到findNet里面去
                    result+=stri+"\n";
                    return result;
                }
            }
        }
        throw string("NET cannot found");
    }
};
