#pragma once
#include <string>
#include <vector>
using namespace std;

class qstring;

typedef vector<qstring> qstringList;

class qstring
{
private:
    qstring _replace(const string& old_value, const string& new_value)
    {
        while(true)
        {
            string::size_type pos(0);
            if( (pos=str.find(old_value)) != string::npos )
            {
                str.replace(pos,old_value.size(),new_value);
            }
            else
                break;
        }
        return str;
    }

    qstringList _split(string qseperator)
    {
        string s=this->str;
        string seperator=qseperator;

        qstringList result;
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
          if(i != j)
          {
            string r=s.substr(i, j-i);
            result.push_back(qstring(r));
            i = j;
          }
        }
        return result;
    }

public:
    string str;

    qstring() : str("") {}
    qstring(const string a) : str(a) {}
    qstring(const char* a) : str(a) {}
    qstring(const qstring & a) : str(a.str) {}
    qstring(char c) : str(to_string(c-48)) {}
    static qstring number(int a) { return qstring(to_string(a)); }

    qstring replace(const qstring& old_value, const qstring& new_value)
    {
        return this->_replace(old_value.str, new_value.str);
    }

    qstring operator[] (int i)    //返回引用，这样才可以对返回值赋值
    {
        return qstring(this->str[i]);
    }

    qstring& operator= (const qstring& a)
    {
        this->str=a.str;
        return *this;
    }

    qstring& operator= (const string &a)
    {
        this->str=a;
        return *this;
    }

    //bool operator== (const string& a) { return this->str==a; }
    bool operator== (const qstring& a) { return this->str==a.str; }
    bool operator!= (const qstring& a) { return this->str!=a.str; }

    qstring& operator+= (const qstring& a)
    {
        this->str+=a.str;
        return *this;
    }

    char operator[](unsigned int i)
    {
        return this->str[i];
    }

    bool find(const qstring & a)
    {
        return this->str.find(a.str)!=this->str.npos;
    }

    float toFloat() { return atof(this->str.c_str()); }

    operator string() { return this->str; }

    bool operator<(const qstring& a) const { return this->str<a.str; }
    qstring operator+ (const qstring &a) { return qstring(this->str+a.str); }
    //QString operator+ (const string &a) { return QString(this->str+a); }
    friend qstring operator+(const string& a, const qstring& b) { return qstring(a+b.str); }

    unsigned int size() { return this->str.size(); }
    qstringList split(qstring qseperator) { return this->_split(qseperator.str); }
    int toInt() { return atoi(this->str.c_str()); }
    qstring mid(unsigned int sub) { return this->str.substr(sub); }
};
