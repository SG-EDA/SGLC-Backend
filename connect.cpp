#include "layout.h"
#include <iostream>
using namespace std;

tuple<float,float> minSwap(float a,float b) //小的放前面
{
    if(a>b)
        return make_tuple(b,a);
    else
        return make_tuple(a,b);
}

optional<rect> layout::checkNewLine(list<line> &newLine, pinRect *r1, pinRect *r2)
{
    for(line& l : newLine)
    {
        optional<rect> result=this->checkLine(l,r1,r2);
        if(result.has_value())
            return result;
    }
    return optional<rect>();
}

//检查一条导线是否有碰撞，如果有碰撞，返回绕过后的整体导线组（list）
optional<list<line>> layout::fixConnect(line l,LEF::metal m1,LEF::metal realM2, pinRect *r1, pinRect *r2) //算上异常返回有三种情况：没有（新导线组）、有和暂时无解（抛出异常）
{
    auto obsRect=this->checkLine(l,r1,r2);
    if(obsRect.has_value())
    {
        rect border=obsRect.value().getOuterBorder(l.metal.spacing);
        list<line> newLine; //新的导线组
        //进行修正
        //1.（绕过障碍矩形/走到障碍矩形的一个距目标rect最近的顶点，并在此处重新向原方向走线）
        if((border.p1.y < l.y1) && (border.p2.y > l.y2)) //横向
        {
            if(l.startPosX < border.p1.x)    //从左至右
            {
                if(m1.vertical == true) //竖直线是m1
                {
                    newLine.push_back(line(l.x1                        , l.y1               , (border.p1.x-(l.y2-l.y1)/2) , l.y2                        , realM2 , l.x1                        , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , ((l.y2+l.y1)/2)    , border.p1.x                 , (border.p2.y+(l.y2-l.y1)/2) , m1     , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , border.p2.y        , l.x2                        , (border.p2.y+(l.y2-l.y1))   , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) , l.x2                        , (border.p2.y+(l.y2-l.y1)/2) ));
                }
                else    //竖直线是realM2
                {
                    newLine.push_back(line(l.x1                        , l.y1               , (border.p1.x-(l.y2-l.y1)/2) , l.y2                        , m1     , l.x1                        , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , ((l.y2+l.y1)/2)    , border.p1.x                 , (border.p2.y+(l.y2-l.y1)/2) , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , border.p2.y        , l.x2                        , (border.p2.y+(l.y2-l.y1))   , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) , l.x2                        , (border.p2.y+(l.y2-l.y1)/2) ));
                }
            }
            else    //从右至左
            {
                if(m1.vertical == true) //竖直线是m1
                {
                    newLine.push_back(line(border.p2.x+(l.y2-l.y1)/2 , l.y1            , l.x2                        , l.y2                        , realM2 , l.x2                        , (l.y1+l.y2)/2               , (border.p2.x+(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));
                    newLine.push_back(line(border.p2.x               , ((l.y2+l.y1)/2) , (border.p2.x+(l.y2-l.y1))   , (border.p2.y+(l.y2-l.y1)/2) , m1     , (border.p2.x+(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                    newLine.push_back(line(l.x1                      , border.p2.y     , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1))   , realM2 , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) , l.x1                        , (border.p2.y+(l.y2-l.y1)/2) ));
                }
                else    //竖直线是realM2
                {
                    newLine.push_back(line(border.p2.x+(l.y2-l.y1)/2 , l.y1            , l.x2                        , l.y2                        , m1     , l.x2                        , (l.y1+l.y2)/2               , (border.p2.x+(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));
                    newLine.push_back(line(border.p2.x               , ((l.y2+l.y1)/2) , (border.p2.x+(l.y2-l.y1))   , (border.p2.y+(l.y2-l.y1)/2) , realM2 , (border.p2.x+(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) ));
                    newLine.push_back(line(l.x1                      , border.p2.y     , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1))   , m1     , (border.p2.x+(l.y2-l.y1)/2) , (border.p2.y+(l.y2-l.y1)/2) , l.x1                        , (border.p2.y+(l.y2-l.y1)/2) ));
                }
            }
        }
        else //纵向
        {
            if(l.startPosY < border.p1.y)   //从下至上
            {
                if(m1.vertical == true) //竖直线是m1
                {
                    newLine.push_back(line(l.x1                        , l.y1                        , l.x2          , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1+l.x2)/2               , l.y1                        , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1))   , (l.x1+l.x2)/2 , border.p1.y                 , realM2 , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , (border.p1.y-(l.x2-l.x1)/2) , border.p1.x   , l.y2 		               , m1     , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , l.y2                        ));
                }
                else    //竖直线是realM2
                {
                    newLine.push_back(line(l.x1                        , l.y1                        , l.x2          , (border.p1.y-(l.x2-l.x1)/2) , realM2 , (l.x1+l.x2)/2               , l.y1                        , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1))   , (l.x1+l.x2)/2 , border.p1.y                 , m1     , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , (border.p1.y-(l.x2-l.x1)/2) , border.p1.x   , l.y2 		               , realM2 , (border.p1.x-(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , l.y2                        ));
                }
            }
            else    //从上至下
            {
                if(m1.vertical == true) //竖直线是m1
                {
                    newLine.push_back(line(l.x1                        , (border.p2.y+(l.x2-l.x1)/2) , l.x2          , l.y2 		               , m1     , (l.x1+l.x2)/2               , l.y2                        , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , border.p2.y                 , (l.x1+l.x2)/2 , (border.p2.y+(l.x2-l.x1))   , realM2 , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , l.y1                        , border.p1.x   , (border.p2.y+(l.x2-l.x1)/2) , m1     , (border.p1.x-(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , l.y1                        ));
                }
                else    //竖直线是realM2
                {
                    newLine.push_back(line(l.x1                        , (border.p2.y+(l.x2-l.x1)/2) , l.x2          , l.y2 		               , realM2 , (l.x1+l.x2)/2               , l.y2                        , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1)/2) , border.p2.y                 , (l.x1+l.x2)/2 , (border.p2.y+(l.x2-l.x1))   , m1     , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) ));
                    newLine.push_back(line((border.p1.x-(l.x2-l.x1))   , l.y1                        , border.p1.x   , (border.p2.y+(l.x2-l.x1)/2) , realM2 , (border.p1.x-(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) , (border.p1.x-(l.x2-l.x1)/2) , l.y1                        ));
                }
            }
        }
        //检查1是否修复成功
        optional<rect> aboveObsRect=checkNewLine(newLine,r1,r2);
        if(!aboveObsRect.has_value())
            return optional<list<line>>(newLine);
        else
        {
            newLine.clear();
            //2.如果1的走线结果依然存在碰撞，向反方向走线
            if((border.p1.y < l.y1) && (border.p2.y > l.y2)) //横向
            {
                if(l.startPosX < border.p1.x)    //从左至右
                {
                    if(m1.vertical == true) //竖直线是m1
                    {
                        newLine.push_back(line(l.x1                        , l.y1                        , (border.p1.x-(l.y2-l.y1)/2) , l.y2          , realM2 , l.x1                        , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));//画到障碍矩形的x1
                        newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , (border.p1.y-(l.y2-l.y1)/2) , border.p1.x                 , (l.y1+l.y2)/2 , m1     , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));//绕线宽度和旧导线保持一致
                        newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1))   , l.x2	                       , border.p1.y   , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) , l.x2                        , (border.p1.y-(l.y2-l.y1)/2) ));//y累加
                    }
                    else    //竖直线是realM2
                    {
                        newLine.push_back(line(l.x1                        , l.y1                        , (border.p1.x-(l.y2-l.y1)/2) , l.y2          , m1     , l.x1                        , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               ));
                        newLine.push_back(line((border.p1.x-(l.y2-l.y1))   , (border.p1.y-(l.y2-l.y1)/2) , border.p1.x                 , (l.y1+l.y2)/2 , realM2 , (border.p1.x-(l.y2-l.y1)/2) , (l.y1+l.y2)/2               , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));
                        newLine.push_back(line((border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1))   , l.x2	                       , border.p1.y   , m1     , (border.p1.x-(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) , l.x2                        , (border.p1.y-(l.y2-l.y1)/2) ));
                    }
                }
                else    //从右至左
                {
                    if(m1.vertical == true) //竖直线是m1
                    {
                        newLine.push_back(line((border.p2.x+(l.y2-l.y1)/2) , l.y1                        , l.x2	                       , l.y2          , realM2 , l.x2                        , (l.y1 + l.y2)/2             , (border.p2.x+(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             ));//画到障碍矩形的x1
                        newLine.push_back(line(border.p2.x                 , (border.p1.y-(l.y2-l.y1)/2) , (border.p2.x+(l.y2-l.y1))   , (l.y2+l.y1)/2 , m1     , (border.p2.x+(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             , (border.p2.x+(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));//绕线宽度和旧导线保持一致
                        newLine.push_back(line(l.x1                        , (border.p1.y-(l.y2-l.y1))   , (border.p2.x+(l.y2-l.y1)/2) , border.p1.y   , realM2 , (border.p2.x+(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) , l.x1                        , (border.p1.y-(l.y2-l.y1)/2) ));//y累加
                    }
                    else    //竖直线是realM2
                    {
                        newLine.push_back(line((border.p2.x+(l.y2-l.y1)/2) , l.y1                        , l.x2	                       , l.y2          , m1     , l.x2                        , (l.y1 + l.y2)/2             , (border.p2.x+(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             ));
                        newLine.push_back(line(border.p2.x                 , (border.p1.y-(l.y2-l.y1)/2) , (border.p2.x+(l.y2-l.y1))   , (l.y2+l.y1)/2 , realM2 , (border.p2.x+(l.y2-l.y1)/2) , (l.y1 + l.y2)/2             , (border.p2.x+(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) ));
                        newLine.push_back(line(l.x1                        , (border.p1.y-(l.y2-l.y1))   , (border.p2.x+(l.y2-l.y1)/2) , border.p1.y   , m1     , (border.p2.x+(l.y2-l.y1)/2) , (border.p1.y-(l.y2-l.y1)/2) , l.x1                        , (border.p1.y-(l.y2-l.y1)/2) ));
                    }
                }
            }
            else //纵向
            {
                if(l.startPosY < border.p1.y)   //从下至上
                {
                    if(m1.vertical == true) //竖直线是m1
                    {
                        newLine.push_back(line(l.x1          , l.y1                        , l.x2                        , (border.p1.y-(l.x2-l.x1)/2) , m1     , (l.x1+l.x2)/2               , l.y1                        , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) ));
                        newLine.push_back(line((l.x1+l.x2)/2 , (border.p1.y-(l.x2-l.x1))   , (border.p2.x+(l.x2-l.x1)/2) , border.p1.y                 , realM2 , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                        newLine.push_back(line(border.p2.x   , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1))   , l.y2                        , m1     , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , l.y2                        ));
                    }
                    else    //竖直线是realM2
                    {
                        newLine.push_back(line(l.x1          , l.y1                        , l.x2                        , (border.p1.y-(l.x2-l.x1)/2) , realM2 , (l.x1+l.x2)/2               , l.y1                        , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) ));
                        newLine.push_back(line((l.x1+l.x2)/2 , (border.p1.y-(l.x2-l.x1))   , (border.p2.x+(l.x2-l.x1)/2) , border.p1.y                 , m1     , (l.x1+l.x2)/2               , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) ));
                        newLine.push_back(line(border.p2.x   , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1))   , l.y2                        , realM2 , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y-(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , l.y2                        ));
                    }
                }
                else    //从上至下
                {
                    if(m1.vertical == true) //竖直线是m1
                    {
                        newLine.push_back(line(l.x1          , (border.p2.y+(l.x2-l.x1)/2) , l.x2                        , l.y2 		               , m1     , (l.x1+l.x2)/2               , l.y2                        , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) ));
                        newLine.push_back(line((l.x1+l.x2)/2 , border.p2.y                 , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y+(l.x2-l.x1))   , realM2 , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) ));
                        newLine.push_back(line(border.p2.x   , l.y1                        , (border.p2.x+(l.x2-l.x1))   , (border.p2.y+(l.x2-l.x1)/2) , m1     , (border.p2.x+(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , l.y1                        ));
                    }
                    else    //竖直线是realM2
                    {
                        newLine.push_back(line(l.x1          , (border.p2.y+(l.x2-l.x1)/2) , l.x2                        , l.y2 		               , realM2 , (l.x1+l.x2)/2               , l.y2                        , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) ));
                        newLine.push_back(line((l.x1+l.x2)/2 , border.p2.y                 , (border.p2.x+(l.x2-l.x1)/2) , (border.p1.y+(l.x2-l.x1))   , m1     , (l.x1+l.x2)/2               , (border.p2.y+(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) ));
                        newLine.push_back(line(border.p2.x   , l.y1                        , (border.p2.x+(l.x2-l.x1))   , (border.p2.y+(l.x2-l.x1)/2) , realM2 , (border.p2.x+(l.x2-l.x1)/2) , (border.p2.y+(l.x2-l.x1)/2) , (border.p2.x+(l.x2-l.x1)/2) , l.y1                        ));
                    }
                }
            }
            optional<rect> belowObsRect=checkNewLine(newLine,r1,r2);
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
                           LEF::metal m1,LEF::metal realM2, list<line> &alreadyLine, int layer,
                       pinRect *r1, pinRect *r2)
{
    if(fabs(p1x-p2x)<0.001 && fabs(p1y-p2y)<0.001) //不需要下一根导线了，递归出口
        return GENRET();

    //求第一条导线
    line l1;
    float a,b;
    if(m1.vertical == true)
    {
        tie(a,b)=minSwap(p1y,p2y);
        l1 = line(p1x,a,b,m1,p1x,p1y,p1x,p2y,true);
    }
    else
    {
        tie(a,b)=minSwap(p1x,p2x);
        l1 = line(p1y,a,b,m1,p1x,p1y,p2x,p1y,false);
    }

    auto pushAllLine=[&alreadyLine](list<line> &allL) {
        for(line l : allL)
            alreadyLine.push_back(l);
    };

    //检查l1是否碰撞
    try {

    auto fixResult=fixConnect(l1,m1,realM2,r1,r2);
    if(!fixResult.has_value())
    {
        alreadyLine.push_back(l1); //确认无误添加
        cout<<l1.endPosX<<" "<<l1.endPosY<<" "<<p2x<<" "<<p2y<<endl;
        return this->genLine(l1.endPosX,l1.endPosY,p2x,p2y,realM2,m1,alreadyLine,layer+1,r1,r2);
    }
    else
    {
        //最开始创建的l1是最短路径，如果绕了，终点一定变了，所以不用考虑这组导线直接到终点的情况
        list<line> newLine=fixResult.value();
        pushAllLine(newLine); //1、2种情况，确认无误添加
        line lastLine=newLine.back();
        //连l2，此时l2起点变了（递归）
        cout<<lastLine.endPosX<<" "<<lastLine.endPosY<<" "<<p2x<<" "<<p2y<<endl;
        return this->genLine(lastLine.endPosX,lastLine.endPosY,p2x,p2y,realM2,m1,alreadyLine,layer+1,r1,r2);
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


void layout::connect(LEF::pin &p1, LEF::pin &p2, vector<line> &allLine, vector<via> &allVia)
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
    GENRET result=this->genLine(p1x,p1y,p2x,p2y,m1,realM2,alreadyLine,1,r1,r2);
    if(result.layer==-1) //无问题
    {
        for(line l : alreadyLine)
            allLine.push_back(l);
    }
    else
    {
        for(int i=1;i<=100;i++) //有问题（情况3、4）循环尝试
        {
            if(i==100) //最后一次循环
            {
                //上下都不行，换层或换pin
                tie(m1,realM2)=this->switchMetal(m1,realM2);
                i=0;
            }

            if(result.layer!=1) //l2遇到问题就把没问题的线先搞进去（l1遇到问题就是全清）
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
                        //上下都不行，换层或换pin
                        tie(m1,realM2)=this->switchMetal(m1,realM2);
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
                        //上下都不行，换层或换pin
                        tie(m1,realM2)=this->switchMetal(m1,realM2);
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
                        //上下都不行，换层或换pin
                        tie(m1,realM2)=this->switchMetal(m1,realM2);
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
                        //上下都不行，换层或换pin
                        tie(m1,realM2)=this->switchMetal(m1,realM2);
                    }
                }
                //根据之前的布线重定义起点
                line lastLine=allLine.back();
                p1x=lastLine.endPosX;
                p2x=lastLine.endPosY;
            }

            result=this->genLine(p1x,p1y,p2x,p2y,m1,realM2,alreadyLine,1,r1,r2);

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
    //如果第一根或最后一根导线的层和pin的层不一样，先在pin上打孔
    auto putViaToPin=[this,&allVia](line &l, pinRect* r, LEF::pin &p)
    {
        if(l.metal!=p.metal)
        {
            float x,y;
            tie(x,y)=l.getCrossCenter(*r);
            //pin基本都在第一层，所以min那个一定是pin的
            //把这via放置到xy
            for(int i=1;i<l.metal.ID;i++)
            {
                LEF::via v=this->lp.getVia(i);
                allVia.push_back(via(x,y,v,this->lp)); //fix:不能全放一个位置！这里还需要一个能决定放哪的策略
            }
        }
    };
    putViaToPin(allLine[0],r1,p1);
    putViaToPin(allLine.back(),r2,p2);
    //打导线之间的孔
    for(int i=1;i<allLine.size();i++)
    {
        //每个和它前一个连
        line &l1=allLine[i-1];
        line &l2=allLine[i];
        float x,y;
        tie(x,y)=l1.getCrossCenter(l2);
        //找到两根导线中下层的via
        int minViaID,maxViaID;
        tie(minViaID,maxViaID)=minSwap(l1.metal.ID,l2.metal.ID);
        //把这via放置到xy
        for(int i=minViaID;i<maxViaID;i++)
        {
            LEF::via v=lp.getVia(i);
            allVia.push_back(via(x,y,v,this->lp)); //fix:不能全放一个位置！这里还需要一个能决定放哪的策略
        }
    }
}
