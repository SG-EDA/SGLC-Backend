#pragma once
#include "lefParser.h"
#include "defParser.h"
#include <queue>

typedef tuple< optional<rect>, optional<rect> > ABRect;
struct GENRET
{
    ABRect e;
    int layer=-1; //-1代表布线成功
};

class layout
{
private:
    optional<rect> checkLine(line l)
    {
        rect result;
        for(LEF::cell &c : this->allCell) //fix:可以尝试只检测近邻的以剪枝
        {
            auto result=l.checkOBS(c);
            if(result.has_value())
                return result;
        }

        rect lr=l.toRect();
        for(line &li : this->allLine) //fix:剪枝同上
        {
            if(l.metal==li.metal)
            {
                rect lir=li.toRect();
                if(lr.isIntersect(lir,l.metal.spacing,l.metal.width))
                {
                    if(lir.isLowerLeft(result)) //新找到的在左下
                        result=lir;
                }
            }
        }

        if(result.isNull())
            return optional<rect>();
        else
            return result;
    }

    void connect(LEF::pin &p1, LEF::pin &p2)
    {
        //查找二者最近的两个rect
        pinRect *r1;
        pinRect *r2;
        tie(r1,r2,ignore)=this->getPinDist(p1,p2);

        float p1x,p1y;
        tie(p1x,p1y)=r1->getMidPos();
        float p2x,p2y;
        tie(p2x,p2y)=r2->getMidPos();
        //暂时在这个位置标记r1 r2占用
        r1->isOccupy=true;
        r2->isOccupy=true;
        //生成line矩形 l1 l2（孔暂不生成，最后调？）
        list<line> alreadyLine;
        GENRET result=this->genLine(p1x,p1y,p2x,p2y,p1.metal,p2.metal,alreadyLine,1);
        if(result.layer==-1) //无问题
        {
            for(line l : alreadyLine)
                this->allLine.push_back(l);
        }
        else if(result.layer==1) //l1遇到问题
        {
            alreadyLine.clear();
            optional<rect> aboveObsRect,belowObsRect;
            tie(aboveObsRect,belowObsRect)=result.e;
            //fix:情况3和4，改变l1起点
        }
        else //l2遇到问题
        {
            //把没问题的线先搞进去
            for(line l : alreadyLine)
                this->allLine.push_back(l);
            //fix:情况3和4，改变l2终点
        }
    }

    LEF::pin& findLEFPin(QString instName, QString pinName)
    {
        for(LEF::cell &c : this->allCell)
        {
            if(c.instName==instName)
            {
                for(LEF::pin &p : c.allPin)
                {
                    if(p.name==pinName)
                        return p;
                }
            }
        }
    }

    static tuple<pinRect*,pinRect*,float> getPinDist(LEF::pin &p1, LEF::pin &p2)
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
        int p1_rect_cnt_last;
        int p2_rect_cnt_last;

        pinRect* p1_rect;
        pinRect* p2_rect;
        //循环比较
        for(int p1_rect_cnt=0;p1_rect_cnt<p1.allRect.size();p1_rect_cnt++)
        {
            p1_rect = &(p1.allRect[p1_rect_cnt]);
            if(p1_rect->isOccupy)
                continue;

            for(int p2_rect_cnt=1;p2_rect_cnt<p2.allRect.size();p2_rect_cnt++)
            {
                if(p1_rect_cnt==p2_rect_cnt)
                    continue;

                p2_rect = &(p2.allRect[p2_rect_cnt]);

                if(p2_rect->isOccupy)
                    continue;

                tie(mid_p1_x,mid_p1_y)=p1_rect->getMidPos();
                tie(mid_p2_x,mid_p2_y)=p2_rect->getMidPos();

                div_x = mid_p1_x - mid_p2_x;
                div_y = mid_p1_y - mid_p2_y;

                div = abs(div_x) + abs(div_y);

                if((p1_rect_cnt == 0) && (p2_rect_cnt == 1))
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

        p1_rect = &(p1.allRect[p1_rect_cnt_last]);
        p2_rect = &(p2.allRect[p2_rect_cnt_last]);
        return make_tuple(p1_rect,p2_rect,last);
    }

    vector<LEF::pin> toLEFPinVec(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin> result;
        for(auto i : allPin)
            result.push_back(this->findLEFPin(i.instName,i.pinName));
        return result;
    }

    vector<LEF::pin> sortAllPin(const vector<DEF::pin> &allPin)
    {
        vector<LEF::pin> LEFallPin=this->toLEFPinVec(allPin);
        float distance = 0;
        float last_distance = 0;
        int min_num = 0;
        for (int i = 0; i < LEFallPin.size() - 2; i++)
        {
            for (int j = i + 1; j < LEFallPin.size() - 1; j++)
            {
                tie(ignore,ignore,distance) = this->getPinDist(LEFallPin[i], LEFallPin[j]);
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
            LEF::pin temp = LEFallPin[min_num];
            LEFallPin[min_num] = LEFallPin[i+1];
            LEFallPin[i+1] = temp;
        }
        return LEFallPin;
    }

    //genLine需要的
    optional<rect> checkNewLine(list<line> &newLine);
    optional<list<line>> fixConnect(line l);
    GENRET genLine(float p1x,float p1y,float p2x,float p2y,
                LEF::metal m1,LEF::metal m2, list<line> &alreadyLine, int layer); //返回值为递归层数

public:
    defParser dp;
    lefParser lp;
    vector<line> allLine; //将net转化为line
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
    }

    void connectAllNet()
    {
        for(DEF::net &n : dp.allNet)
        {
            auto LEFallPin=this->sortAllPin(n.allPin);
            for(int i=1;i<LEFallPin.size();i++)
            {
                this->connect(LEFallPin[i-1],LEFallPin[i]);
            }
        }
    }
};

