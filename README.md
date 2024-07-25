# Slave Control Program

## Prerequisite

1. Install [Visual Studio 2019](https://visualstudio.microsoft.com/zh-hans/thank-you-downloading-visual-studio/?sku=Professional&rel=16)；
2. Install [TwinCAT3 XAE](https://www.beckhoff.com.cn/zh-cn/support/download-finder/search-result/?c-1=26782567)；
3. 为了方便后续的version control，请按照[version_control_guide](./SlaveControlProgram/Docs/version_control_guide.md)中的`Prerequisite`完成设置；

## Brief

下位机程序的demo版本，目前连接的硬件有控制面板、电机；

具体实现的功能大致包括：

1. 读取MotionControl需要的[配置信息](./SlaveControlProgram/SlaveControlProgram/Portal/MotionControl.xml)；
2. 接受上位机指令，如状态机跳转信号，运动指令结构体等；
3. Ring Buffer存储上位机的指令信息；
4. 接收IO信号，发送IO信号等；

## ADS Data Interface

PLC数据类型和C++数据类型的映射关系[:arrow_right:](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_plc_intro/2529399691.html&id=2025525145742283335)
| 序号 | 端口号| 变量名 | 类型 | 大小 | Linked To | 备注 | 用途 |
| :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| 1 | 851 | MAIN.fb_CommandProcess.mInputData | ARRAY [0..1023] OF MotionCommand | 256000.0 | 上位机写入 | \ | 存储host指令的Buffer |
| 2 | 851 | MAIN.fb_CommandProcess.iWriteIndex | LINT | 8.0 | 上位机写入 | \ | host指令的个数 |
| 3 | 851 | MAIN.fb_CommandProcess.iReadIndex | LINT | 8.0 | 上位机读取 | \ | 下位机消耗指令的个数 |
| 4 | 851 | MAIN.fb_CommandProcess.mOutputData | MotionCommand | 256.0 | CommandData | \ | 从Buffer中取出的指令 |
| 5 | 351 | CommandData | MotionCommand | 256.0 | MAIN.fb_CommandProcess.mOutputData | \ | 发送给电机的指令 |
| 6 | 851 | \ | \ | \ | \ | \ | No |
| 7 | 851 | \ | \ | \ | \ | \ | No |
| 8 | 851 | \ | \ | \ | \ | \ | No |

## Configuration

Choose Target System选项中选择激活程序的目标设备，可以是笔记本(选择Local)也可以是远程连接的工控机，目前仅有Device-001工控机是连接硬件设备的，若要将程序激活在其他未直连硬件的设备上，需要先在工程里[禁用设备](https://tr.beckhoff.com.cn/mod/data/view.php?d=19&rid=1303)，而后生成解决方案，点击[Activate Configuration](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_c/110705163.html&id=)，若目标机器右下角状态栏内TwinCAT图标变为<font color = "Green">绿色</font>，即说明程序激活成功。

## Scope Server Project

1. 确保目标工控机上安装了TC3ScopeExportTool.exe，路径应如下所示：

    C:\TwinCAT\Functions\TE130X-Scope-View\TC3ScopeExportTool.exe

    若未安装，请从下方链接下载后安装到目标工控机上：

    https://www.beckhoff.com/en-en/products/automation/twincat/tfxxxx-twincat-3-functions/tf3xxx-measurement/tf3300.html

2. 打开ScopeServerProject，将DataPool内的每个变量的属性中的TargetSystem都改为目标工控机；

3. 找到[RecordData.tcscopex](./SlaveControlProgram/ScopeServerProject/RecordData.tcscopex)，将其复制到目标工控机的文件夹"C:/RecordData/ScopeServer/";

4. 找到[ScopeExporterSettings.xml](./SlaveControlProgram/ScopeServerProject/ScopeExporterSettings.xml)，将其复制到目标工控机的文件夹"C:/RecordData/Settings/";

5. 记录完毕之后，在目标工控机的文件夹"C:\RecordData\SVDX"内找到后缀.svdx的文件，拷贝到笔记本后打开，即可看到上一次运行过程中被记录变量的数值变化曲线；在目标工控机的文件夹"C:\RecordData\CSV"内找到后缀.csv的文件，即可看到被记录变量的数值；

6. 若要增减观察记录的变量，需要同步更新步骤3、4中的文件到目标工控机中；

## Portal Project Brief

### mInput

1. 接收部分硬件的输入信号, 如控制面板等；
2. xxxxxx

### mOutput

1. 将上位机传入的指令信号通过变量链接的形式传给C++模块的Input;
2. 给部分硬件输出指令信号，如控制面板的按键指示灯；
3. xxxxxx

## MotionControl Project Brief

### Input

1. 接收电机驱动器的输入信号，如编码器位置、速度、力矩、错误码等，计算并更新轴和关节的反馈位置(单位: mm)、扭矩(单位：1/1000)和速度(单位: mm/s)
2. xxxxxx

#### Data Area -- Inputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| CommandData | MotionCommand | 256 | MAIN.fb_CommandProcess.mOutputData | 接收PLC收到的application发送的运动指令 | \ |
| MotionCtrlParam | ARRAY [0..3,0..0] OF MotionControlInfo | 224 | MAIN.fb_MotorInfoLoad.McInfo | 接收PLC解析的MotionControl.xml | \ |
| DriverInputs | ARRAY [0..3,0..0] OF DriverInput | 112 | 后续会链到I/O中电机的Input通道 | 接收驱动器的反馈信息 | \ |
| PanelInformation | PanelInfo | 64 | 后续会链到PLC中相同类型的变量 | 接收面板按钮的状态信息 | \ |
| mockButton | ARRAY [0..9] OF BOOL | 10 | application写入 | 触发状态跳转 | \ |
| TestInputs | ARRAY [0..19] OF LREAL | 160 | \ | 测试写入用，后续会删除 | \ |

### StateControl

1. 接受状态跳转指令，根据状态不同，接收或者生成对应的控制指令，包含控制字，位置/速度/扭矩;
2. xxxxxx

### Output

1. 将StateControl中的控制指令转换成输出给电机驱动器的指令信号，如控制字，目标位置、速度、扭矩等
2. 生成错误码，传递给上位机；
3. xxxxxx

#### Data Area -- Outputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| DriverOutputs | ARRAY [0..3,0..0] OF DriverOutput | 64 | 后续会链到I/O中电机的Output通道 | 发送指令给驱动器 | \ |
| TestOutputs | ARRAY [0..19] OF LREAL | 160 | \ | 测试，观测变量用，后续会删除 | \ |

### Safety Module

1. 将每个电机运动的上下限位存储在[MotionControl.xml](./SlaveControlProgram/SlaveControlProgram/Portal/MotionControl.xml)内，见变量UpperPosLimit, LowerPosLimit；扭矩限幅见变量TorPdoMax；
2. 具体实现在[Axis](./SlaveControlProgram/SlaveControlProgram/MotionControl/Axis/Axis.cpp)类内的`SendTargetPosition`函数，在发送位置指令前会将指令位置限制在上下限位内；`SendTargetTorque`函数，在发送扭矩指令前会将指令扭矩限制在上下限幅内；

### Log Module

目前实现了动态生成错误码的功能[GenerateErrorCode](./SlaveControlProgram/SlaveControlProgram/MotionControl/)，错误码共有8位，每一位存储不同的信息：

1. 工程编号，‘3’代表三轴机床，‘4’代表四轴机床，‘5’代表五轴机床；
2. 发生错误的模块，‘0’代表系统模块，‘1’代表运动控制模块；
3. 模块产生的具体错误信息，如‘0’代表位置超限，‘1’代表速度超限等；
4. 发生错误的轴编号；
5. 发生错误的电机编号；
6. 预留位，后续拓展使用；
7. 预留位，后续拓展使用；
8. 错误等级，有A、B、C、D、E五个等级；

