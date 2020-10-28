SGLC-Backend
===========
本项目针对集成电路线网布线问题，完成了以下工作：
* 一套用于电路构成元素和约束条件表示的数据结构。可以覆盖本次比赛测试样例中所使用的METAL、VIA、CELL等电路构成元素
* 一套针对LEF和DEF文件的parse库。可以覆盖本次比赛测试样例中使用的LEF、DEF语法子集，将代码文本所描述的电路结构、元件和约束条件信息转换到上一条所说的数据结构表示
* 用于电路布线的功能模块。针对DEF描述的元件放置与连接关系，根据LEF中描述的元件属性与约束条件进行布线（放置导线和via）

电路表示与Parse
--------------
由于我们希望减少算法执行过程中冗余的信息，因此需要自行设计数据结构进行电路表示（这样可以在布线时更快地查找相关信息）。不过这样就无法使用cadence的开源parser。因此我们给出了一个相较cadence parser更简洁的parser实现，它使用简单的状态机堆叠构成，高效地对DEF和LEF代码进行分析，并将分析结果转换成我们需要的数据结构。

下面将分别介绍我们的**电路表示结构**与**Parser设计**

### 电路表示结构
我们的数据结构分为对DEF表示结构和对LEF表示结构两部分。DEF表示结构通过DEF parser对DEF代码进行分析而实例化，LEF表示结构则是通过分析LEF代码而实例化

#### DEF表示结构

##### component类
描述被放置于版图中的元件（对应DEF文件的`COMPONENTS`代码块）
###### 属性
``` cpp
QString instName
```
component对象的名字。这是component对象在DEF文件中的标识代号
``` cpp
QString cellName
```
这个（种）元件的名字。在LEF表示结构中查询这个名字，可以得到关于这个（种）元件的具体描述
```
float x
    float y
```
component对象在版图中被放置的位置
```
QString dire
```
component对象在版图中被放置的方向

##### pin类
描述某个component对象上的一个接口，一般用于表示导线的终端
###### 属性
``` cpp
QString instName
```
component对象的名字
``` cpp
QString pinName
```
接口的名字。

通过component对象名查询到component对象，再通过component对象的`cellName`属性查询LEF表示结构中的cell对象，即可在cell对象中通过`pinName`查询到对应cell中这个接口的相关信息

##### net类
