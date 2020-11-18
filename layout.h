#pragma once
#include "lefParser.h"
#include "defParser.h"
#include <queue>

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
    QString getName() { return "via"+QString::number(v.m1); }

    QString getPos()
    {
        return "( "+QString::number(int(this->x))+" "+QString::number(int(this->y))+" )";
    }
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
        for(vector<line> &allLine : this->allNetLine)
        {
            for(line &li : allLine) //fix:剪枝同上
            {
                if(l.metal==li.metal)
                {
                    rect lir=li.toRect();
                    if(lr.isIntersect(lir,l.metal.spacing,l.width))
                    {
                        if(lir.isLowerLeft(result)) //新找到的在左下
                            result=lir;
                    }
                }
            }
        }

        if(result.isNull())
            return optional<rect>();
        else
            return result;
    }

    void connect(LEF::pin &p1, LEF::pin &p2, vector<line> &allLine, vector<via> &allVia)
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
        //找到要走线的层
        LEF::metal m1=p1.metal;
        //求反向导线的金属层
        LEF::metal realM2;
        if(p2.metal.ID<m1.ID) //使得第二条导线离m2的层尽量近
            realM2=lp.getMetal(m1.ID-1);
        else
            realM2=lp.getMetal(m1.ID+1);
        //生成line矩形 l1 l2（孔在下一步生成）
        list<line> alreadyLine;
        GENRET result=this->genLine(p1x,p1y,p2x,p2y,m1,realM2,alreadyLine,1);
        if(result.layer==-1) //无问题
        {
            for(line l : alreadyLine)
                allLine.push_back(l);
        }
        else 
        {
            while(1) //有问题（情况3、4）循环尝试
            {
                if(result.layer==2) //l2遇到问题就把没问题的线先搞进去（l1遇到问题就是全清）
                {
                    for(line l : alreadyLine)
                        allLine.push_back(l);
                }
                alreadyLine.clear(); //清空，准备在新的起终点布线

                //获取上下无法绕过的矩形
                rect aboveObsRect,belowObsRect;
                tie(aboveObsRect,belowObsRect)=result.e;

                if(result.layer==1) //解决l1遇到的问题，改变l1起点
                {
                    aboveObsRect=aboveObsRect.getOuterBorder(m1.spacing);
                    belowObsRect=belowObsRect.getOuterBorder(m1.spacing);
                    //求新的p1x、p1y
                    if(m1.vertical == true) //判断方向竖直
                    {
                        if(aboveObsRect.p1.x > (r1->p1.x+m1.maxNeedWidth))  //左面的块x1在pin范围内
                        {
                            p1x = aboveObsRect.p1.x-(m1.maxNeedWidth/2);
                        }
                        else if(belowObsRect.p2.x < (r1->p2.x-m1.maxNeedWidth))    //下面的块x2在pin范围内
                        {
                            p1x = belowObsRect.p2.x+(m1.maxNeedWidth/2);
                        }
                        else
                        {
                            throw string("NEED OTHER Metal or Pin"); //fix:上下都不行，换层或换pin
                        }
                    }
                    else    //方向水平
                    {
                        if(aboveObsRect.p2.y < (r1->p2.y-m1.maxNeedWidth))  //上面的块y2在pin范围内
                        {
                            p1y = aboveObsRect.p2.y+(m1.maxNeedWidth/2);
                        }
                        else if(belowObsRect.p1.y > (r1->p1.y+m1.maxNeedWidth))    //下面的块y1在pin范围内
                        {
                            p1y = belowObsRect.p1.y-(m1.maxNeedWidth/2);
                        }
                        else
                        {
                            throw string("NEED OTHER Metal or Pin"); //fix:上下都不行，换层或换pin
                        }
                    }
                }
                else //解决l2遇到的问题，改变l2终点
                {
                    aboveObsRect=aboveObsRect.getOuterBorder(realM2.spacing);
                    belowObsRect=belowObsRect.getOuterBorder(realM2.spacing);
                    //求新的p2x、p2y
                    if(realM2.vertical == true) //判断方向竖直
                    {
                        if(aboveObsRect.p1.x > (r2->p1.x+realM2.maxNeedWidth))  //左面的块x1在pin范围内
                        {
                            p2x = aboveObsRect.p1.x-(realM2.maxNeedWidth/2);
                        }
                        else if(belowObsRect.p2.x < (r2->p2.x-realM2.maxNeedWidth))    //下面的块x2在pin范围内
                        {
                            p2x = belowObsRect.p2.x+(realM2.maxNeedWidth/2);
                        }
                        else
                        {
                            throw string("NEED OTHER Metal or Pin"); //fix:上下都不行，换层或换pin
                        }
                    }
                    else    //方向水平
                    {
                        if(aboveObsRect.p2.y < (r2->p2.y-realM2.maxNeedWidth))  //上面的块y2在pin范围内
                        {
                            p2y = aboveObsRect.p2.y+(realM2.maxNeedWidth/2);
                        }
                        else if(belowObsRect.p1.y > (r2->p1.y+realM2.maxNeedWidth))    //下面的块y1在pin范围内
                        {
                            p2y = belowObsRect.p1.y-(realM2.maxNeedWidth/2);
                        }
                        else
                        {
                            throw string("NEED OTHER Metal or Pin"); //fix:上下都不行，换层或换pin
                        }
                    }
                    //根据之前的布线重定义起点
                    line lastLine=allLine.back();
                    p1x=lastLine.endPosX;
                    p2x=lastLine.endPosY;
                }

                GENRET result=this->genLine(p1x,p1y,p2x,p2y,p1.metal,p2.metal,alreadyLine,1);

                //检测这个结果是否可以了
                if(result.layer==-1) //无问题
                {
                    for(line l : alreadyLine)
                        allLine.push_back(l);
                    break;
                }
            }
        }
        //生成孔
        for(int i=1;i<allLine.size();i++)
        {
            //每个和它前一个连
			line &l1=allLine[i-1];
			line &l2=allLine[i];
			float x,y;
            tie(x,y)=l1.getCrossCenter(l2);
            //找到对应层的via
            LEF::via v;
            if(l1.metal.ID<l2.metal.ID)
                v=lp.getVia(l1.metal.ID);
            else
                v=lp.getVia(l2.metal.ID);
            //fix:因为目前不考虑两根导线跨多层，所以两个导线必然是相邻层，直接getVia就行
            //把这via放置到xy
            allVia.push_back(via(x,y,v,this->lp));
        }
    }

    LEF::pin findLEFPin(QString instName, QString pinName)
    {
        for(LEF::cell &c : this->allCell)
        {
            if(c.instName==instName)
            {
                for(LEF::pin p : c.allPin)
                {
                    if(p.name==pinName)
                        return p;
                }
            }
        }
        throw string("LEF Pin not Found");
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
        for(DEF::pin i : allPin)
        {
            LEF::pin p=this->findLEFPin(i.instName,i.pinName);
            result.push_back(p);
        }
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
    optional<list<line>> fixConnect(line l,LEF::metal m1,LEF::metal realM2);
    GENRET genLine(float p1x, float p1y, float p2x, float p2y,
                LEF::metal m1, LEF::metal realM2, list<line> &alreadyLine, int layer); //返回值为递归层数

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

            if(n.allPin.size()<=1) //和边界pin连的这里不考虑
                continue;

            auto LEFallPin=this->sortAllPin(n.allPin);
            for(int i=1;i<LEFallPin.size();i++)
            {
                this->connect(LEFallPin[i-1],LEFallPin[i],allLine,allVia);
            }
            this->allNetLine.push_back(allLine);
            this->allNetVia.push_back(allVia);
        }
    }
};

