#include <iostream>
#include "codegen.h"
#include <QFile>
#include <QTextStream>

using namespace std;

QString ReadTXT(QString path)
{
    QFile f(path);
    f.open(QIODevice::ReadOnly);
    QTextStream text(&f);
    return text.readAll();
}

void WriteTXT(QString path, QString text)
{
   QFile f(path);
   f.open(QFile::Text|QFile::Append);
   QTextStream out(&f);
   out<<text;
}

int main()
{
    QString def=ReadTXT("D:/sample.def");
    QString lef=ReadTXT("D:/sample.lef");
    defParser p1(def);
    lefParser p2(lef,1,8);
    /*auto c=p2.getCell("CELL2");
    auto v=p2.getVia(6);
    auto m=p2.getMetal(6);*/
    layout l(p1,p2);
    codegen cg(l);
    QString result=cg.genNETS();
    WriteTXT("D:/result.def",result);
    return 0;
}
