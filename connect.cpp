#include "layout.h"
#include <iostream>
using namespace std;

tuple<float,float> minSwap(float a,float b)
{
    if(a>b)
        return make_tuple(b,a);
    else
        return make_tuple(a,b);
}

optional<rect> layout::checkNewLine(list<line> &newLine)
{
    for(line& l : newLine)
    {
        auto result=this->checkLine(l);
        if(result.has_value())
            return result;
    }
    return optional<rect>();
}

//检查一条导线是否有碰撞，如果有碰撞，返回绕过后的整体导线组（list）
optional<list<line>> layout::fixConnect(line l,LEF::metal m1,LEF::metal realM2) //算上异常返回有三种情况：没有（新导线组）、有和暂时无解（抛出异常）
{
    auto obsRect=this->checkLine(l);
    if(obsRect.has_value())
    {
        rect border=obsRect.value().getOuterBorder(l.metal.spacing);
        list<line> newLine; //新的导线组
        //进行修正
        //1.（绕过障碍矩形/走到障碍矩形的一个距目标rect最近的顶点，并在此处重新向原方向走线）
        if((border.p1.y < l.y1) && (border.p2.y > l.y2)) //横向
        {
            if(m1.vertical == true)
            {
                newLine.push_back(line(l.x1                        , l.y1               , (border.p1.x-(l.y2-l.y1)/2) , l.y2                         , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1 + l.y2)/2              ));
                newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , ((l.y2+l.y1)/2)    , border.p1.x                 , (border.p2.y+(l.y2-l.y1)/2) , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , border.p2.y        , l.x2                        , (border.p2.y+(l.y2-l.y1))    , realM2 , l.x2                        , (border.p2.y+(l.y2-l.y1)/2)  ));
            }
            else
            {
                newLine.push_back(line(l.x1                        , l.y1               , (border.p1.x-(l.y2-l.y1)/2) , l.y2                         , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1 + l.y2)/2              ));
                newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , ((l.y2+l.y1)/2)    , border.p1.x                 , (border.p2.y+(l.y2-l.y1)/2) , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , border.p2.y        , l.x2                        , (border.p2.y+(l.y2-l.y1))    , realM2 , l.x2                        , (border.p2.y+(l.y2-l.y1)/2)  ));
            }
        }
        else //纵向
        {
            if(m1.vertical == true)
            {
                newLine.push_back(line(l.x1                        , l.y1                        , l.x2            , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1 + l.x2)/2             , (border.p1.y-(l.x2-l.x1)/2) ));
                newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1))   , (l.x1 + l.x2)/2 , border.p1.y                 , realM2 , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , (border.p1.y-(l.x2-l.x1)/2) , border.p1.x     , l.y2 		                 , m1     , (border.p1.x-(l.x2-l.x1)/2) , l.y2                        ));
            }
            else
            {
                newLine.push_back(line(l.x1                        , l.y1                        , l.x2            , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1 + l.x2)/2             , (border.p1.y-(l.x2-l.x1)/2) ));
                newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1))   , (l.x1 + l.x2)/2 , border.p1.y                 , realM2 , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , (border.p1.y-(l.x2-l.x1)/2) , border.p1.x     , l.y2 		                 , m1     , (border.p1.x-(l.x2-l.x1)/2) , l.y2                        ));
            }
        }
        //检查1是否修复成功
        auto aboveObsRect=checkNewLine(newLine);
        if(!aboveObsRect.has_value())
            return optional<list<line>>(newLine);
        else
        {
            newLine.clear();
            //2.如果1的走线结果依然存在碰撞，向反方向走线
            if((border.p1.y < l.y1) && (border.p2.y > l.y2)) //横向
            {
                if(m1.vertical == true)
                {
                    newLine.push_back(line(l.x1                        , l.y1                        , (border.p1.x-(l.y2-l.y1)/2) , l.y2            , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             ));//画到障碍矩形的x1
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , (border.p1.y-(l.y2-l.y1)/2) , border.p1.x                 , (l.y1 + l.y2)/2 , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));//绕线宽度和旧导线保持一致
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1))   , l.x2	                       , border.p1.y     , realM2 , l.x2                        , (border.p1.y-(l.y2-l.y1)/2) ));//y累加
                }
                else
                {
                    newLine.push_back(line(l.x1                        , l.y1                        , (border.p1.x-(l.y2-l.y1)/2) , l.y2            , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , (border.p1.y-(l.y2-l.y1)/2) , border.p1.x                 , (l.y1 + l.y2)/2 , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1))   , l.x2	                       , border.p1.y     , realM2 , l.x2                        , (border.p1.y-(l.y2-l.y1)/2) ));
                }
            }
            else //纵向
            {
                if(m1.vertical == true)
                {
                    newLine.push_back(line(l.x1            , l.y1                        , l.x2                         , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1 + l.x2)/2             , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((l.x1 + l.x2)/2 , (border.p1.y-(l.x2-l.x1))   , (border.p2.x +(l.x2-l.x1)/2) , border.p1.y                 , realM2 , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line(border.p2.x     , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1))    , l.y2                        , m1     , (border.p2.x+(l.x2-l.x1)/2) , l.y2                        ));
                }
                else
                {
                    newLine.push_back(line(l.x1            , l.y1                        , l.x2                         , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1 + l.x2)/2             , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((l.x1 + l.x2)/2 , (border.p1.y-(l.x2-l.x1))   , (border.p2.x +(l.x2-l.x1)/2) , border.p1.y                 , realM2 , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line(border.p2.x     , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1))    , l.y2                        , m1     , (border.p2.x+(l.x2-l.x1)/2) , l.y2                        ));
                }
            }
            auto belowObsRect=checkNewLine(newLine);
            if(!belowObsRect.has_value()) //检查2是否修复成功
                return optional<list<line>>(newLine);
            else
            {
                //返回aboveObsRect和belowObsRect
                throw make_tuple(aboveObsRect.value(),belowObsRect.value()); //暂时无解，回到上层搞
                //3.如果2的走线结果依然存在碰撞，移动l的起始位置到障碍区域之上，此时l1起点变了（递归）
                //这个应该返回到递归上一层处理（throw个异常，外面返回false）
                //4.如果3的走线结果依然存在碰撞，移动l的其实位置到障碍区域之下，此时l1起点变了（递归）
                //同上
            }
        }
    }
    else
        return optional<list<line>>();
}


GENRET layout::genLine(float p1x,float p1y,float p2x,float p2y,
                           LEF::metal m1,LEF::metal realM2, list<line> &alreadyLine, int layer)
{
    if(abs(p1x-p2x)<0.001 && abs(p1y-p2y)<0.001) //不需要下一根导线了，递归出口
        return GENRET();

    //求第一条导线
    line l1;
    float a,b;
    if(m1.vertical == true)
    {
        tie(a,b)=minSwap(p1y,p2y);
        l1 = line(p1x,a,b,m1,p1x,p2y,true);
    }
    else
    {
        tie(a,b)=minSwap(p1x,p2x);
        l1 = line(p1y,a,b,m1,p2x,p1y,false);
    }

    auto pushAllLine=[&alreadyLine](list<line> &allL) {
        for(line l : allL)
            alreadyLine.push_back(l);
    };

    //检查l1是否碰撞
    try {

    auto fixResult=fixConnect(l1,m1,realM2);
    if(!fixResult.has_value())
    {
        alreadyLine.push_back(l1); //确认无误添加
        cout<<l1.endPosX<<" "<<l1.endPosY<<" "<<p2x<<" "<<p2y<<endl;
        return this->genLine(l1.endPosX,l1.endPosY,p2x,p2y,realM2,m1,alreadyLine,layer+1);
    }
    else
    {
        //最开始创建的l1是最短路径，如果绕了，终点一定变了，所以不用考虑这组导线直接到终点的情况
        list<line> newLine=fixResult.value();
        pushAllLine(newLine); //1、2种情况，确认无误添加
        line lastLine=newLine.back();
        //连l2，此时l2起点变了（递归）
        cout<<lastLine.endPosX<<" "<<lastLine.endPosY<<" "<<p2x<<" "<<p2y<<endl;
        return this->genLine(lastLine.endPosX,lastLine.endPosY,p2x,p2y,realM2,m1,alreadyLine,layer+1);
    }

    }
    catch (tuple<rect,rect> e) {
        //表明遇到3、4情况。返回无法处理的边缘障碍矩形和本层的layer（layer=1为l1遇到问题，2为l2遇到问题）
        GENRET result;
        result.layer=layer;
        result.e=e;
        return result;
    }
}
