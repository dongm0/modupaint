# 说明
* modu2geom已废弃，untangle.cpp已废弃，modu.cpp的旧功能(Addcell，Untangle，SaveCache，LoadCache，以及坐标移动都已废弃)。
* 思路是，依次加入单元，计算一个初始位置，再使用mesquite优化。
* inputfile里有部分paint的输入示例，construction17和18是17转18的模板，con17和con18是可用于paint的格式。
* 优化的方法是untanglebeta各个点+B3全局。

# Todo
* 优化的部分挑出来做成wrapper使用
* main部分需要改一下结构，main函数里不需要控制的部分
* modu类中的不需要的成员变量删掉
* 我自己Untangle的函数需要改成两边
