SGLC-Backend
===========
本项目针对集成电路线网布线问题，完成了以下工作：
* 完成了一套用于电路构成元素和约束条件表示的数据结构。可以覆盖本次比赛测试样例中所使用的METAL、VIA、CELL等电路构成元素
* 完成了一套针对LEF和DEF文件的parse库。可以覆盖本次比赛测试样例中使用的LEF、DEF语法子集，将代码文本所描述的电路结构、元件和约束条件信息转换到上一条所说的数据结构表示
* 完成了用于电路布线的功能模块。针对DEF描述的元件放置与连接关系，根据LEF中描述的元件属性与约束条件进行布线（放置导线和via）

电路表示与Parse
--------------
由于我们希望减少算法执行过程中冗余的信息，因此需要自行设计数据结构进行电路表示（这样可以在布线时更快地查找相关信息）。不过这样就无法使用cadence的开源parser。因此我们给出了一个相较cadence parser更简洁的parser实现，它使用简单的状态机堆叠构成，高效地对DEF和LEF代码进行分析，并将分析结果转换成我们需要的数据结构。

下面将分别介绍我们的**电路表示结构**与**Parser设计**

电路表示结构
---------
我们的数据结构分为对DEF表示结构和对LEF表示结构两部分。DEF表示结构通过DEF parser对DEF代码进行分析而实例化，LEF表示结构则是通过分析LEF代码而实例化。DEF表示了一个实际电路的元件的摆放与连接关系，LEF则表示了METAL、VIA、CELL等电路元件与布线所需部件的属性。

### DEF表示结构

#### component类
描述被放置于版图中的元件（对应DEF文件的`COMPONENTS`代码块）
##### 属性
``` cpp
QString instName
```
component对象的名字。这是component对象在DEF文件中的标识名字
``` cpp
QString cellName
```
这个（种）元件的名字。在LEF表示结构中查询这个名字，可以得到关于这个（种）元件的具体描述

举个例子，元件`CELL1`可以在版图中被放置多个，它们可以被命名为`inst1`和`inst2`。那么`CELL1`就是`cellName`，`inst1`就是`instName`

```
float x
float y
```
component对象在版图中被放置的位置
```
QString dire
```
component对象在版图中被放置的方向

#### pin类
描述某个component对象上的一个接口，一般用于表示导线的终端。这个类仅记录component对象名字和接口名字，关于这个接口的具体信息需要到LEF表示中查询
##### 属性
``` cpp
QString instName
```
component对象的名字
``` cpp
QString pinName
```
接口的名字

通过component对象名查询到component对象，再通过component对象的`cellName`属性查询LEF表示结构中的cell对象，即可在cell对象中通过`pinName`查询到对应cell中这个接口的相关信息

#### net类
描述一组接口间的连接（对应DEF文件的`NETS`代码块）
##### 属性
``` cpp
QString name
```
这组连接的名字
``` cpp
vector<pin> allPin
```
连接的所有接口（将这些接口连接在一起）

### LEF表示结构

#### metal类
描述版图某一金属层的属性
##### 属性
``` cpp
int m
```
金属层对象的ID
``` cpp
float width
```
本金属层中导线的宽度
``` cpp
float spacing
```
本金属层中导线的最小间距（中心线距离）
``` cpp
float area=-1
```
单条导线的最小面积约束（`-1`表示无约束）
``` cpp
bool vertical
```
本层导线是水平走线还是竖直走线，`true`为水平走线
##### 方法
``` cpp
QString getName()
```
如`m=1`，那么这个函数将返回`METAL1`。`METAL+金属层ID`是LEF文件中对金属层的标识名字。有时需要使用这个名字进行一些查询

#### via类
描述穿透连接两层金属的孔
##### 属性
``` cpp
int m1
```
起始金属层ID
``` cpp
int m2
```
目标金属层ID（一般是相邻的）
``` cpp
float spacing
```
孔所占的面积

#### cell类
描述一个元件
##### 属性
``` cpp
QString cellName
```
（这种）元件的名字
``` cpp
QString instName
```
元件对象在DEF中的标识名字。每个`LEF::cell`对象都对应了一个`DEF::component`对象，可以用这个名字实现二者间的查询。可以参照上一节中的**component类**小节
``` cpp
float sizeA1
float sizeA2
```
元件对象的长与宽
``` cpp
vector<pin> allPin
```
元件对象中的所有接口（这个`pin`是下小节所说的`LEF::pin`，不是前面的`DEF::pin`）
``` cpp
obs o
```
元件中所有不可走线的障碍区域（不同层的障碍区域在obs对象中分别存储）
##### 方法
``` cpp
void setToLayout(float setX, float setY, QString dire)
```
已知元件对象被放置到版图的坐标和方向时，将该对象的所有接口和障碍区域rect按`dire`所描述的方向进行旋转，并转换到版图坐标系

#### pin类
描述一个接口（接口对象会组合到cell对象当中）。与DEF表示中的pin类不同的是，这个类中存储接口的具体属性
##### 属性
``` cpp
QString name
```
接口的名字
``` cpp
QString layer
```
接口所在层的名字（为`metal::getName()`的返回值格式）
``` cpp
vector<rect> allRect
```
构成接口的所有rect。如果导线想要与这个接口连接，与其中任意一个rect连接即可
##### 方法
``` cpp
void setToLayout(float setX, float setY, QString dire)
```
已知**接口对象所在的cell**被放置到版图的坐标和方向时，将该接口中所有rect的坐标按`dire`所描述的方向进行旋转，并转换到版图坐标系

Parser
----------
我们实现了两个不同的parser分别解析DEF代码和LEF代码。它们都使用简洁的状态机堆叠构成。

### defParser
#### 方法
``` cpp
defParser(QString code)
```
构造parser对象时传入DEF代码文本内容。构造函数中会直接进行parse，将分析结果直接存入本对象的`allComponent`、`allPin`（本题暂不考虑）和`allNet`数据成员中

Parser分为扫描COMPONENTS和扫描NETS两个子状态，分别对应COMPONENTS和NETS代码块。COMPONENTS代码块中有多个component的描述；NETS代码块中有多个net的描述，每个net由多个`DEF::pin`组成。在进入状态后，parser会逐行提取这些component、net（包含其中的pin）的信息，并构造对象，将其存储到数据成员中

### lefParser
#### 方法
``` cpp
lefParser(QString code)
```
构造parser对象时传入LEF代码文本内容。构造函数中仅会对代码进行简单拆分，不进行parse。通过下面介绍的get系列函数获取LEF中某个cell、metal、via的信息
``` cpp
LEF::via getVia(int m1,int m2)
```
获取某个via的信息。如传入参数`m1=1,m2=2`，则会在LEF代码中查找VIA12的信息，构造`LEF::via`对象并返回
``` cpp
LEF::metal getMetal(int _m)
```
获取某个metal的信息。如传入参数`_m=1`，则会在LEF代码中查找METAL1的信息，构造`LEF::metal`对象并返回
``` cpp
LEF::cell getCell(QString cellName)
```
获取某个cell的信息。如传入参数`cellName=”CELL1”`，则会在LEF代码中查找CELL1的信息，构造`LEF::cell`对象并返回。

对cell的解析过程有扫描OBS和扫描PIN两个子状态（由私有成员函数`getOBS`和`getPin`实现），分别对应cell代码块中的`PIN`子代码块（有多个）和OBS子代码块（有一个）。两个子状态函数可以提取cell中pin和OBS的信息，并添加到cell对象的`allPin`和`o`成员中

电路布线模块
-------
布局模块接受DEF parser和LEF parser作为输入。DEF parser中包含对DEF代码分析后得到的电路结构。LEF parser指定了布线所使用的LEF文本，但先不对它进行分析，在布线过程中再对需要知道的信息进行实时查询。

下面将以布线模块的成员函数为单元，对布线过程进行分步说明（按函数调用顺序）

### 构造函数
构造函数用于初始化资源，即向LEF parser查询DEF中所有的component对应的cell。并将**cell对象**和**cell中所有的rect**根据DEF指定的版图坐标和放置方向转换到版图坐标系。

### connectAllNet
`connectAllNet`函数尝试分别连接DEF中各个net下的所有接口（如`net1`下的所有接口要被连接在一起，`net2`下的所有接口要被连接在一起……）。这也是我们的核心任务。

我们采用基于贪心思想的策略：一个接口总是先与离它最近的接口连接。为了达到这一目的，我们首先应当对接口进行排序，如`allPin`是存储一个net下所有接口的数组，那么排序的逻辑为：

* 数组第0个元素不变
* 第1个元素替换为距离第0个元素最近的那个pin
* 第2个元素替换为距离第1个元素最近的那个pin
* ……以此类推

这样排序后，将第`i`接口与第`i+1`个接口进行连接即可。这个功能由`connect`函数完成。

### connect
`connect`函数尝试放置导线和via连接两个接口。

#### 选择起点终点
首先我们要选择走线的起点和终点。由于接口由多个rect构成，将导线连接到任意一个rect都视为对这个接口的连接。那么为了使得导线长度最短，我们选择连接两个接口最近的rect。如接口1在接口2左上，那么连接接口1右下和接口2左上的两个rect。

#### 生成最短路径导线
选择了起点和终点后，先生成最短曼哈顿路线的导线，即：

水平导线和竖直导线需要在不同层走线，我们选择金属层ID尽量小的两层，然后在二者的交叉点放置一个可以连接这两个金属层的via。不过目前放置的这两条导线没有考虑与其它导线和obs区域冲突的问题，首先需要解决冲突。

#### 基于最短路径导线解决冲突
对于目前的水平、竖直两条导线，分别调用`checkLine`函数检查与其碰撞的冲突矩形。

这里说的冲突矩形包含了**obs区域**、**其它接口的rect**和**其它同层导线**。需要注意的是，因为LEF约束了导线间的最短距离，在计算该导线与其它导线是否碰撞时，两条导线的宽都将膨胀`(spacing-width)/2`。如下图所示：

`checkLine`将会返回与其碰撞的“最左下”冲突矩形。如如下图所示的情况：

`checkLine`仅会返回`矩形1`。这是因为解决冲突需要有一个顺序：先解决导线“碰撞”到的第一个矩形。这样会保证所做的工作是有效的。

举个例子：

如果先绕过第二个冲突矩形，那么它左边的导线依然与第一个冲突矩形冲突，此时再解决第一个冲突，不能保证**绕过第一个冲突的导线末端**可以与**绕过第二个冲突矩形的导线首端**顺利连接。

而先绕过第一个冲突矩形后，我们可以重新生成最短路径导线：

然后对新的最短路径导线（`导线2`与`导线3`）检查冲突即可。此时`导线1`一定是可用的。

需要注意的是，这个顺序只需要在解决所有冲突时一致即可，实际可以将“最左下”的作为“第一个冲突矩形”，也可以将“最右上”的作为第一个冲突矩形。

按照此逻辑，可以解决冲突，基于原先最短路径的导线生成符合LEF约束要求的导线。