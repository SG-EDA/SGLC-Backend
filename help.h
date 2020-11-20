#pragma once
#include "QString.h"

class help
{
public:
    static bool VerifyNumber(qstring str)
    {
        std::string temp = str.str;
        for (int i = 0; i < str.size(); i++)
        {
            if ((temp[i]<'0' || temp[i]>'9') && temp[i]!='.')
                return false;
        }
        return true;
    }

    static qstringList splitSpace(qstring stri)
    {
        stri=stri.replace("  "," ");
        return stri.split(" ");
    }

    static qstring getLastElm(qstring stri, qstring keyword)
    {
        qstringList strList=help::splitSpace(stri);
        for(int i=0;i<strList.size();i++)
        {
            if(strList[i]==keyword)
                return strList[i+1];
        }
    }
};
