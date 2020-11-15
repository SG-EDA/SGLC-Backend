#pragma once
#include "layout.h"

class codegen
{
private:
    //处理一个net，在后面添加line和via。返回处理完的行下标
    int genNet(int i,QString &result,vector<line> &allLine, vector<via> &allVia)
    {
        result+=this->l.dp.codeList[i]+"\n"; //把net那行加进去
        i++;

        for(;i<this->l.dp.codeList.length();i++)
        {
            QString stri=this->l.dp.codeList[i];
            result+=stri+"\n";
            //向下探测有没有到结尾
            QString stri2=this->l.dp.codeList[i+1];
            if(stri.indexOf("net")!=-1)
            {
                //到结尾了
                result+="+ ROUTED ";
                bool first=true;
                //把allVia加上
                for(via v : allVia)
                {
                    if(!first)
                        result+="NEW ";
                    else
                        first=false;

                    result=v.m.getName()+" "+v.getPos()+" "+v.getName()+"\n";
                }
                //把allLine加上
                for(line l : allLine)
                {
                    if(!first)
                        result+="NEW ";
                    else
                        first=false;

                    result+=l.metal.getName()+" "+l.getPos1()+" "+l.getPos2()+"\n";
                }
                break;
            }
        }
        result+=";\n";
        return i;
    }

public:
    layout l;
    codegen(layout l) : l(l)
    {

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

    QString genNETS()
    {
        QString result;
        //使用类似NETparser，每个net结束之后添加对应下标的line
        bool findNet=false;
        int netNum=0;
        for(int i=0;i<this->l.dp.codeList.length();i++)
        {
            QString stri=this->l.dp.codeList[i];
            if(stri.indexOf("NETS")!=-1)
                findNet=true;

            if(findNet)
            {
                if(stri.indexOf("net")!=-1) //如果碰到一个net，转到genNet里去把这个处理完
                {
                    i=this->genNet(i,result,l.allNetLine[netNum],l.allNetVia[netNum]);
                    netNum++;
                }

                if(stri.indexOf("END NETS")!=-1)
                {
                    //END这句不会在前面添加到findNet里面去
                    result+=stri+"\n";
                    return result;
                }
            }
        }
        return result;
    }
};
