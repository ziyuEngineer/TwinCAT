# Slave Control Program

## Prerequisite

1. Install [Visual Studio 2019](https://visualstudio.microsoft.com/zh-hans/thank-you-downloading-visual-studio/?sku=Professional&rel=16)；
2. Install [TwinCAT3 XAE](https://www.beckhoff.com.cn/zh-cn/support/download-finder/search-result/?c-1=26782567)；

## Brief

下位机程序的demo版本，目前连接的硬件有控制面板、电机；

具体实现的功能大致包括：

1. 读取MotionControl需要的[配置信息](./MotorTuner/MotorTuner/TestModule/MotionControl.xml)；
2. 接受上位机指令，如状态机跳转信号，运动指令结构体等；
3. Ring Buffer存储上位机的指令信息；
4. 接收IO信号，发送IO信号等；


## ADS Data Interface

| 序号 | 端口号| 变量名 | 类型 | 大小 | Linked To | 备注 | 用途 |
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| 1 | 851 | MAIN.mInputData | ARRAY [0..999] OF ST_DATA | 256000.0 | 上位机写入 | \ | 存储host指令的Buffer |
| 2 | 851 | MAIN.iWriteIndex | LINT | 8.0 | 上位机写入 | \ | host指令的个数 |
| 3 | 851 | MAIN.iReadIndex | LINT | 8.0 | 上位机读取 | \ | 下位机消耗指令的个数 |
| 4 | 851 | MAIN.mOutputData | ST_DATA | 256.0 | CommandData | \ | 从Buffer中取出的指令 |
| 5 | 351 | CommandData | ST_DATA | 256.0 | MAIN.mOutputData | \ | 发送给电机的指令 |
| 6 | 851 | \ | \ | \ | \ | \ | No |
| 7 | 851 | \ | \ | \ | \ | \ | No |
| 8 | 851 | \ | \ | \ | \ | \ | No |


## Configuration

Choose Target System选项中选择激活程序的目标设备，可以是笔记本(选择Local)也可以是远程连接的工控机，目前仅有Device-001工控机是连接硬件设备的，若要将程序激活在其他未直连硬件的设备上，需要先在工程里[禁用设备](https://tr.beckhoff.com.cn/mod/data/view.php?d=19&rid=1303)，而后生成解决方案，点击[Activate Configuration](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_c/110705163.html&id=)，若目标机器右下角状态栏内TwinCAT图标变为<font color = "Green">绿色</font>，即说明程序激活成功。

## Safety Module

1. 将每个电机运动的上下限位存储在[MotionControl.xml](./MotorTuner/MotorTuner/TestModule/MotionControl.xml)内，见变量UpperPosLimit, LowerPosLimit；后续根据实际需求可以添加速度及扭矩限幅；
2. 具体实现在[MotionControl](./MotorTuner/MotorTuner/MotorTuner/MotionControl/MotionControl.cpp)类内的SendRefPos函数，在发送位置指令前都会判断指令位置是否处于上下限位内；

## Log Module

目前实现了动态生成错误码的功能[GenerateErrorCode](./MotorTuner/MotorTuner/MotorTuner/CommonBase.cpp)，错误码共有8位，每一位存储不同的信息：

1. 工程编号，‘3’代表三轴机床，‘4’代表四轴机床，‘5’代表五轴机床；
2. 发生错误的模块，‘0’代表系统模块，‘1’代表运动控制模块；
3. 模块产生的具体错误信息，如‘0’代表位置超限，‘1’代表速度超限等；
4. 发生错误的轴编号；
5. 发生错误的电机编号；
6. 预留位，后续拓展使用；
7. 预留位，后续拓展使用；
8. 错误等级，有A、B、C、D、E五个等级；
