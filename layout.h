#pragma once
#include "lefParser.h"
#include "defParser.h"
#include <list>
#include <time.h>

struct GENRET
{
    tuple<rect,rect> e;
    int layer=-1; //-1代表布线成功
};

class via
{
public:
    float x;
    float y;
    LEF::via v;
    LEF::metal m;
    via(float x,float y, LEF::via v, lefParser &lp) : x(x), y(y), v(v), m(lp.getMetal(v.m1)) {}
    qstring getName() { return "via"+qstring::number(v.m1); }

    qstring getPos()
    {
        return "( "+qstring::number(int(this->x))+" "+qstring::number(int(this->y))+" )";
    }
};

class layout
{
private:
    optional<rect> checkLine(line l, pinRect* r1, pinRect* r2)
    {
        for(LEF::cell &c : this->allCell) //可以尝试只检测近邻的以剪枝
        {
            optional<rect> result=l.checkPinRect(c,r1,r2);
            if(result.has_value())
                return result;
            result=l.checkOBS(c);
            if(result.has_value())
                return result;
        }

        rect lr=l.toRect();
        for(vector<line> &allLine : this->allNetLine)
        {
            for(line &li : allLine) //剪枝同上
            {
                if(l.metal==li.metal)
                {
                    rect lir=li.toRect();
                    if(lr.isIntersect(lir,l.metal.spacing,l.width))
                        return lir;
                }
            }
        }

        return optional<rect>();
    }

    tuple<LEF::metal,LEF::metal> switchMetal(LEF::metal m1, LEF::metal m2)
    {
        int maxNewMetal=max(m1.ID+2,m2.ID+2);
        if(maxNewMetal>=this->lp.allMetal.size())
            throw string("all Metal disabled");
        //都上移两层（因为都是从下层开始的）
        LEF::metal newM1=this->lp.getMetal(m1.ID+2);
        LEF::metal newM2=this->lp.getMetal(m2.ID+2);
        return make_tuple(newM1,newM2);
    }

    void connectPin(LEF::pin &p1, LEF::pin &p2, vector<line> &allLine, vector<via> &allVia);
    bool connectPinRect(LEF::pin &p1, LEF::pin &p2, pinRect *r1, pinRect *r2,
                        vector<line> &allLine, vector<via> &allVia);

    LEF::pin* findLEFPin(qstring instName, qstring pinName)
    {
        for(LEF::cell &c : this->allCell)
        {
            if(c.instName==instName)
            {
                for(LEF::pin &p : c.allPin)
                {
                    if(p.name==pinName)
                        return &p;
                }
            }
        }
        throw string("LEF Pin not Found");
    }

    /*static tuple<pinRect*,pinRect*> randGetPin(LEF::pin &p1, LEF::pin &p2)
    {
        auto random=[](unsigned int n) {
            float num;
            static int i;
            float random;
            srand(((unsigned int)(time(NULL) + i)));
            random = (rand() % n);
            i += 311;
            return int(random);
        };

        while(1)
        {
            pinRect* p1_rect=&p1.allRect[random(p1.allRect.size())];
            pinRect* p2_rect=&p2.allRect[random(p2.allRect.size())];
            if(!p1_rect->isOccupy && !p2_rect->isOccupy)
                return make_tuple(p1_rect,p2_rect);
        }
    }*/

    static tuple<pinRect*,pinRect*,float> getMinDistPinRect(LEF::pin &p1, LEF::pin &p2)
    {
        //rect中点
        float mid_p1_x ;
        float mid_p1_y ;
        float mid_p2_x ;
        float mid_p2_y ;
        //rect差值
        float div_x ;
        float div_y ;
        float div ;
        //上次计算结果
        float last;
        int p1_rect_cnt_last=-1;
        int p2_rect_cnt_last=-1;

        pinRect* p1_rect;
        pinRect* p2_rect;
        //循环比较
        for(int p1_rect_cnt=0;p1_rect_cnt<p1.allRect.size();p1_rect_cnt++)
        {
            p1_rect = &(p1.allRect[p1_rect_cnt]);
            if(p1_rect->isOccupy)
                continue;

            for(int p2_rect_cnt=0;p2_rect_cnt<p2.allRect.size();p2_rect_cnt++)
            {
                p2_rect = &(p2.allRect[p2_rect_cnt]);

                if(p2_rect->isOccupy)
                    continue;

                tie(mid_p1_x,mid_p1_y)=p1_rect->getMidPos();
                tie(mid_p2_x,mid_p2_y)=p2_rect->getMidPos();

                div_x = mid_p1_x - mid_p2_x;
                div_y = mid_p1_y - mid_p2_y;

                div = fabs(div_x) + fabs(div_y);

                if((p1_rect_cnt_last == -1) || (p2_rect_cnt_last == -1))
                {
                    last = div ;
                    p1_rect_cnt_last = p1_rect_cnt;
                    p2_rect_cnt_last = p2_rect_cnt;
                }
                else if(div<last)
                {
                    last = div;
                    p1_rect_cnt_last = p1_rect_cnt;
                    p2_rect_cnt_last = p2_rect_cnt;
                }
            }
        }

        if(p1_rect_cnt_last==-1)
            p1_rect_cnt_last=0;
        if(p2_rect_cnt_last==-1)
            p2_rect_cnt_last=0;

        p1_rect = &(p1.allRect[p1_rect_cnt_last]);
        p2_rect = &(p2.allRect[p2_rect_cnt_last]);
        return make_tuple(p1_rect,p2_rect,last);
    }

    vector<LEF::pin*> toLEFPinVec(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin*> result;
        for(DEF::pin i : allPin)
        {
            LEF::pin* p=this->findLEFPin(i.instName,i.pinName);
            result.push_back(p);
        }
        return result;
    }

    vector<LEF::pin*> sortAllPin(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin*> LEFallPin=this->toLEFPinVec(allPin);
        float distance = 0;
        float last_distance = 0;
        int min_num = 0;
        for (int i = 0; i < LEFallPin.size() - 2; i++)
        {
            for (int j = i + 1; j < LEFallPin.size() - 1; j++)
            {
                tie(ignore,ignore,distance) = this->getMinDistPinRect(*(LEFallPin[i]), *(LEFallPin[j]));
                //在i后面找和i距离最小的
                if ((i == 0) && (j == 1)) {
                    last_distance = distance;
                    min_num = j;
                }
                else if (distance < last_distance) {
                    last_distance = distance;
                    min_num = j;
                }
            }
            LEF::pin* temp = LEFallPin[min_num];
            LEFallPin[min_num] = LEFallPin[i+1];
            LEFallPin[i+1] = temp;
        }
        return LEFallPin;
    }

    //genLine需要的
    optional<rect> checkNewLine(list<line> &newLine, pinRect *r1, pinRect *r2);
    optional<list<line>> fixConnect(line l,LEF::metal m1,LEF::metal realM2, pinRect *r1, pinRect *r2);
    GENRET genLine(float p1x, float p1y, float p2x, float p2y,
                LEF::metal m1, LEF::metal realM2, list<line> &alreadyLine, int layer,
                   pinRect *r1, pinRect *r2);

public:
    defParser dp;
    lefParser lp;
    //vector<line> allLine; //将net转化为line
    vector<vector<line>> allNetLine;
    vector<vector<via>> allNetVia;
    vector<LEF::cell> allCell; //版图中放置的所有cell（转换为版图坐标系）

    layout(defParser dp, lefParser lp) : dp(dp), lp(lp)
    {
        for(DEF::component &c : dp.allComponent)
        {
            LEF::cell lefC=lp.getCell(c.cellName);
            lefC.instName=c.instName;
            lefC.setToLayout(c.x,c.y,c.dire);
            this->allCell.push_back(lefC);
        }
        this->connectAllNet();
    }

    void connectAllNet()
    {
        for(DEF::net &n : dp.allNet)
        {
            vector<line> allLine;
            vector<via> allVia;

            if(n.allPin.size()>1) //两个以上的才需要连
            {
                auto LEFallPin=this->sortAllPin(n.allPin);
                for(int i=1;i<LEFallPin.size();i++)
                {
                    this->connectPin(*(LEFallPin[i-1]),*(LEFallPin[i]),allLine,allVia);
                }
            }

            //所有net按下标都对应一对他俩
            this->allNetLine.push_back(allLine);
            this->allNetVia.push_back(allVia);
        }
    }
};

