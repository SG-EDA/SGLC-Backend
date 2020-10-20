#pragma once
#include <string>
#include <vector>
using namespace std;

class QString;

typedef vector<QString> QStringList;

class QString
{
public:
    string str;

    QString(const string a) : str(a) {}
    QString(const QString & a) : str(a.str) {}

    QString& operator= (const string& a)
    {
        this->str=a;
        return *this;
    }

    QString& operator= (const QString& a)
    {
        this->str=a.str;
        return *this;
    }

    bool find(const QString & a)
    {
        return this->str.find(a.str)!=this->str.npos;
    }

    float toFloat() { return atof(this->str.c_str()); }

    operator string() { return this->str; }

    QStringList split(QString qs, QString qseperator)
    {
        string s=qs;
        string seperator=qseperator;

        QStringList result;
        typedef string::size_type string_size;
        string_size i = 0;

        while(i != s.size()){
          //找到字符串中首个不等于分隔符的字母；
          int flag = 0;
          while(i != s.size() && flag == 0){
            flag = 1;
            for(string_size x = 0; x < seperator.size(); ++x)
            if(s[i] == seperator[x]){
              ++i;
              flag = 0;
              break;
            }
          }

          //找到又一个分隔符，将两个分隔符之间的字符串取出；
          flag = 0;
          string_size j = i;
          while(j != s.size() && flag == 0){
            for(string_size x = 0; x < seperator.size(); ++x)
            if(s[j] == seperator[x]){
              flag = 1;
              break;
            }
            if(flag == 0)
            ++j;
          }
          if(i != j){
            result.push_back(s.substr(i, j-i));
            i = j;
          }
        }
        return result;
    }
};
