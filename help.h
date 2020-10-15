#pragma once
#include <QString>
#include <QStringList>

class help
{
public:
    static bool VerifyNumber(QString str)
    {
        std::string temp = str.toStdString();
        for (int i = 0; i < str.length(); i++)
        {
            if ((temp[i]<'0' || temp[i]>'9') && temp[i]!='.')
                return false;
        }
        return true;
    }

    static QStringList splitSpace(QString stri)
    {
        stri=stri.replace("  "," ");
        return stri.split(" ");
    }

    static QString getLastElm(QString stri, QString keyword)
    {
        QStringList strList=help::splitSpace(stri);
        for(int i=0;i<strList.length();i++)
        {
            if(strList[i]==keyword)
                return strList[i+1];
        }
    }
};
