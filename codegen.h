#pragma once
#include "layout.h"

class codegen
{
private:
    QString genNet(vector<line> &allLine)
    {

    }

public:
    layout l;
    codegen(layout l) : l(l)
    {

    }

    QString genNONDEFAULTRULES()
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
    }

    QString genNETS()
    {
        //使用类似NETparser，每个net结束之后添加对应下标的line
    }
};
