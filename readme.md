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

Parser分为COMPONENTS和NETS两个主状态，分别对应`COMPONENTS`和`NETS`代码块。`COMPONENTS`代码块中有多个component的描述，`NETS`

### lefParser
#### 方法
