#pragma once
#include <string>
#include <vector>
using namespace std;

class QString;

typedef vector<QString> QStringList;

class QString
{
private:
    QString _replace(const string& old_value, const string& new_value)
    {
        while(true)
        {
            string::size_type pos(0);
            if( (pos=str.find(old_value)) != string::npos )
            {
                str.replace(pos,old_value.length(),new_value);
            }
            else
                break;
        }
        return str;
    }

    QStringList _split(string qseperator)
    {
        string s=this->str;
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

public:
    string str;

    QString() : str("") {}
    QString(const string a) : str(a) {}
    QString(const char* a) : str(a) {}
    QString(const QString & a) : str(a.str) {}
    QString(char c) : str(to_string(c)) {}
    static QString number(int a) { return QString(to_string(a)); }

    QString replace(const QString& old_value, const QString& new_value)
    {
        return this->_replace(old_value.str, new_value.str);
    }

    QString& operator= (const QString& a)
    {
        this->str=a.str;
        return *this;
    }

    //bool operator== (const string& a) { return this->str==a; }
    bool operator== (const QString& a) { return this->str==a.str; }
    bool operator!= (const QString& a) { return this->str!=a.str; }

    QString& operator+= (const QString& a)
    {
        this->str+=a.str;
        return *this;
    }

    char operator[](unsigned int i)
    {
        return this->str[i];
    }

    bool find(const QString & a)
    {
        return this->str.find(a.str)!=this->str.npos;
    }

    float toFloat() { return atof(this->str.c_str()); }

    operator string() { return this->str; }

    QString operator+ (const QString &a) { return QString(this->str+a.str); }
    //QString operator+ (const string &a) { return QString(this->str+a); }
    friend QString operator+(const string& a, const QString& b) { return QString(a+b.str); }

    unsigned int size() { return this->str.size(); }
    QStringList split(QString qseperator) { this->_split(qseperator.str); }
    int toInt() { return atoi(this->str.c_str()); }
    QString mid(unsigned int sub) { return this->str.substr(sub); }
};
