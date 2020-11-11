#include "layout.h"

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
optional<list<line>> layout::fixConnect(line l) //算上异常返回有三种情况：没有（新导线组）、有和暂时无解（抛出异常）
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
            newLine.push_back(line(l.x1                      , l.y1 , border.p1.x  , l.y2                     , l.metal));//画到障碍矩形的x1
            newLine.push_back(line((border.p1.x-(l.y2-l.y1)) , l.y2 , border.p1.x  , border.p2.y                , l.metal));//绕线宽度和旧导线保持一致
            newLine.push_back(line((border.p1.x-(l.y2-l.y1)) , border.p2.y, l.x2   , (border.p2.y+(l.y2-l.y1))  , l.metal));//y累加
        }
        else //纵向
        {
            newLine.push_back(line(l.x1                      , l.y1                      , l.x2        , border.p1.y   , l.metal));
            newLine.push_back(line(border.p1.x               , (border.p1.y-(l.x2-l.x1)) , l.x1        , border.p1.y   , l.metal));
            newLine.push_back(line((border.p1.x-(l.x2-l.x1)) , (border.p1.y-(l.x2-l.x1)) , border.p1.x , l.y2 		, l.metal));
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
                newLine.push_back(line(l.x1                    , l.y1                        , border.p1.x , l.y2      , l.metal));//画到障碍矩形的x1
                newLine.push_back(line((border.p1.x-(l.y2-l.y1)) , border.p1.y               , border.p1.x , l.y1      , l.metal));//绕线宽度和旧导线保持一致
                newLine.push_back(line((border.p1.x-(l.y2-l.y1)) , (border.p1.y-(l.y2-l.y1)) , l.x2	                   , border.p1.y , l.metal));//y累加
            }
            else //纵向
            {
                newLine.push_back(line(l.x1        , l.y1                      , l.x2                      , border.p1.y , l.metal));
                newLine.push_back(line(l.x2        , (border.p1.y-(l.x2-l.x1)) , border.p2.x               , border.p1.y , l.metal));
                newLine.push_back(line(border.p2.x , (border.p1.y-(l.x2-l.x1)) , (border.p2.x+(l.x2-l.x1)) , l.y2        , l.metal));
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
                           LEF::metal m1,LEF::metal m2, list<line> &alreadyLine, int layer)
{
    if(p1x==p2x && p1y==p2y) //不需要下一根导线了，递归出口
        return GENRET();

    //求反向导线的金属层
    LEF::metal realM2;
    if(m2.ID<m1.ID) //使得第二条导线离m2的层尽量近
        realM2=lp.getMetal(m1.ID-1);
    else
        realM2=lp.getMetal(m1.ID+1);

    //求第一条导线
    line l1;
    float a,b;
    if(m1.vertical == true)
    {
        tie(a,b)=minSwap(p1y,p2y);
        l1 = line(p1x,a,b,m1,true);
    }
    else
    {
        tie(a,b)=minSwap(p1x,p2x);
        l1 = line(p1y,a,b,m1,false);
    }

    auto pushAllLine=[&alreadyLine](list<line> allL) {
        for(line l : allL)
            alreadyLine.push_back(l);
    };

    //检查l1是否碰撞
    try {

    auto fixResult=fixConnect(l1);
    if(!fixResult.has_value())
    {
        alreadyLine.push_back(l1); //确认无误添加
        return this->genLine(l1.x2,l1.y2,p2x,p2y,m1,m2,alreadyLine,layer+1);
    }
    else
    {
        //最开始创建的l1是最短路径，如果绕了，终点一定变了，所以不用考虑这组导线直接到终点的情况
        list<line> newLine=fixResult.value();
        pushAllLine(newLine); //1、2种情况，确认无误添加
        auto lastLine=newLine.back();
        //连l2，此时l2起点变了（递归）
        return this->genLine(lastLine.x2,lastLine.y2,p2x,p2y,m1,m2,alreadyLine,layer+1);
    }

    }
    catch (ABRect e) {
        //表明遇到3、4情况。返回无法处理的边缘障碍矩形和本层的layer（layer=1为l1遇到问题，2为l2遇到问题）
        GENRET result;
        result.layer=layer;
        result.e=e;
        return result;
    }
}