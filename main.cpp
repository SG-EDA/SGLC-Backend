#include <iostream>
#include "defParser.h"
#include "lefParser.h"
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

int main()
{
    QString def=ReadTXT("D:/sample.def");
    QString lef=ReadTXT("D:/sample.lef");
    defParser p1(def);
    lefParser p2(lef);
    auto c=p2.getCell("CELL2");
    return 0;
}
