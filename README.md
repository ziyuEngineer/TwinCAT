# Slave Control Program

## Prerequisite

1. 安装Visual Studio 2019和TwinCAT3，参考[安装配置教程](docs/install.md)。
2. 为了方便后续的version control，请按照[version_control_guide](./SlaveControlProgram/Docs/version_control_guide.md)中的`Prerequisite`完成设置；

## Brief

下位机程序，目前连接的硬件有控制面板、X、Y、Z轴的四个电机(倍福)，1个主轴电机(松下)；

具体实现的功能大致包括：

1. 读取MotionControl需要的[配置信息](./SlaveControlProgram/SlaveControlProgram/Portal/AxisGroup.xml)；
2. 接受上位机指令，如状态机跳转信号，运动指令结构体等；
3. Ring Buffer存储上位机的指令信息；
4. 接收IO信号，发送IO信号等；

## ADS Data Interface

PLC数据类型和C++数据类型的映射关系[:arrow_right:](https://infosys.beckhoff.com/english.php?content=../content/1033/tc3_plc_intro/2529399691.html&id=2025525145742283335)


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

#### Data Area -- PLC Inputs

| 变量名 | 类型 | 大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| MAIN.fb_CommandProcess.mInputData | ARRAY [0..1023] OF MotionCommand | 262144.0 | 上位机写入 | 存储host指令的Buffer | \ |
| MAIN.fb_CommandProcess.iWriteIndex | UDINT | 4.0 | 上位机写入 | host指令的个数 | \ |
| MAIN.fb_PanelInfoProcess.pInput | ARRAY [0..5] OF USINT | 6.0 | PANEL_Inputs | 获取面板输入通道的值 | \ |
| MAIN.fb_PanelInfoProcess.handwheelCurrPos | USINT | 1.0 | TEACH_HANDWHEEL . PANEL_Inputs . Panel . Device 3 (EtherCAT) . Devices | 获取手轮当前位置 | \ |
| MAIN.fb_SoEProcess.stPlcDriveRef | ARRAY [0..3,0..0] OF Tc2_EtherCAT.ST_PlcDriveRef | 40.0 | (netId/port/Chn).AdsAddr.InfoData.ZY | 访问驱动器 | \ |
| MAIN.mSystemState | StateMachine | 12.0 | StateMachine  | 系统当前状态及各状态下的标志位 | \ |
| \ | \ | \ | \ | \ | \ |

### mStateControl

| 系统状态 | PLC 执行任务 |
| :----: | :----: |
| Idle | \ |
| Initialization | 面板处理功能块初始化；SoE地址信息获取 |
| Disabled | \ |
| Standby | 重置RingBuffer；手摇功能所需数据重置 |
| Moving | 将RingBuffer中数据发送给C++模块 |
| Handwheel | \ |
| LimitViolation | \ |
| Fault | 重置SoE |
| Emergency | \ |
| Test | \ |

### mOutput

1. 将上位机传入的指令信号通过变量链接的形式传给C++模块的Input;
2. xxxxxx

#### Data Area -- PLC Outputs

| 变量名 | 类型 | 大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| MAIN.fb_CommandProcess.iReadIndex | UDINT | 4.0 | 上位机读取 | 下位机消耗指令的个数 | \ |
| MAIN.fb_CommandProcess.mOutputData | MotionCommand | 256.0 | CommandData | 从Buffer中取出的指令 | 、 |
| MAIN.fb_PanelInfoProcess.pOutput | ARRAY [0..5] OF USINT | 6.0 | PANEL_Outputs  | 给面板输出通道发送值 | \ |
| MAIN.fb_PanelInfoProcess.panelInfoOutput | PanelInfo | 56.0 | PanelInformation  | 给C++端发送面板信息，如按钮状态，手轮行程等 | \ |
| MAIN.mockButtons | ARRAY [0..9] OF BOOL | 10.0 | mockButtons  | 测试用，链接到C++中对应变量 | \ |
| MAIN.Outputs2Cpp | ARRAY [0..19] OF LREAL | 160.0 | TestInputs  | 测试用，链接到C++中对应变量，用于切换Test模式下的测试用例 | \ |

## MotionControl Project Brief

### Input

1. 接收电机驱动器的输入信号，如编码器位置、速度、力矩、错误码等，计算并更新轴和关节的反馈位置(单位: mm)、扭矩(单位：1/1000)和速度(单位: mm/s)
2. xxxxxx

#### Data Area -- C++ Inputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| CommandData | MotionCommand | 256。0 | MAIN.fb_CommandProcess.mOutputData | 接收PLC收到的application发送的运动指令 | \ |
| MotionCtrlParam | ARRAY [0..5] OF MotionControlInfo | 384.0 | MAIN.fb_MotorInfoLoad.McInfo | 接收PLC解析MotionControl.xml得到的轴参数 | 后续弃用 |
| AxisInputs | ARRAY [0..9] OF DriverInput | 240.0 | I/O中电机的Input通道 | 接收驱动器的反馈信息 | \ |
| PanelInformation | PanelInfo | 56.0 | MAIN.fb_PanelInfoProcess.panelInfoOutput | 接收面板按钮的状态信息等 | \ |
| mockButton | ARRAY [0..9] OF BOOL | 10.0 | application写入 | 触发状态跳转 | \ |
| TestInputs | ARRAY [0..19] OF LREAL | 160.0 | \ | 测试写入用，后续会删除 | \ |

#### 以下为每个muckButton对应的功能

| mockButton按键编号 | 功能 |
| :----: | :----: |
| 0 | \ |
| 1 | \ |
| 2 | \ |
| 3 | \ |
| 4 | Standby状态下，若为True，状态跳转至Moving |
| 5 | \ |
| 6 | Emergency状态下，若为True，状态跳转至Disabled <br> LimitViolation状态下，若为True，状态跳转至Standby <br> Fault状态下，若为True，状态跳转至Standby|
| 7 | \ |
| 8 | \ |
| 9 | Standby状态下，若为True，状态跳转至Test |

### StateControl

1. 接受状态跳转指令，根据状态不同，接收或者生成对应的控制指令，包含控制字，位置/速度/扭矩;
2. xxxxxx

| 系统状态 | C++ 执行任务 |
| :----: | :----: |
| Idle | \ |
| Initialization | 接收m_Parameters中的轴控制参数，更新AxisGroup和Spindle对应电机的成员变量 |
| Disabled | 等待使能信号，使能成功后跳转至Standby |
| Standby | 使电机位置保持，等待状态跳转信号 |
| Moving | 接收PLC端传递的控制指令，使电机完成对应运动 |
| Handwheel | 接收PLC端发送的手摇行程，使电机完成相对运动 |
| LimitViolation | 等待Reset信号，跳转至Standby |
| Fault | 等待Reset信号，发送指令到PLC端，使其重置SoE，清错后跳转至Standby |
| Emergency | 松开急停后，跳转至Disabled状态 |
| Test | 测试模式，根据输入的测试用例编号完成指定测试 |

### Output

1. 将StateControl中的控制指令转换成输出给电机驱动器的指令信号，如控制字，目标位置、速度、扭矩等
2. 生成错误码，传递给上位机；
3. xxxxxx

#### Data Area -- C++ Outputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| AxisOutputs | ARRAY [0..9] OF DriverOutput | 160.0 | I/O中电机的Output通道 | 发送指令给驱动器 | \ |
| AxisInfoOutputs | ARRAY [0..19] OF LREAL | 160.0 | \ | 测试，观测变量用，后续会删除 | \ |
| StateMachine | SystemState | 12.0 | MAIN.mSystemState | 系统当前状态及各状态下的标志位 | \ |

#### 以下为StateMachine展开后各变量的说明

| 变量名 | 说明 | 功能 |
| :----: | :----: | :----: |
| CurrentState | 系统当前状态 | \ |
| StateFlag[0] | Idle标志位 | 暂未启用 |
| StateFlag[1] | Initialization标志位 | 暂未启用 |
| StateFlag[2] | Disabled标志位 | 暂未启用 |
| StateFlag[3] | Standby标志位 | 暂未启用 |
| StateFlag[4] | Moving标志位 | 暂未启用 |
| StateFlag[5] | Handwheel标志位 | 暂未启用 |
| StateFlag[6] | LimitViolation标志位 | 暂未启用 |
| StateFlag[7] | Fault标志位 | 通知PLC端重置SoE以清除错误 |
| StateFlag[8] | Emergency标志位 | 暂未启用 |
| StateFlag[9] | Test标志位 | 暂未启用 |

### Safety Module

1. 将每个电机运动的上下软限位对应的编码器数值存储在[AxisGroup.xml](./SlaveControlProgram/SlaveControlProgram/Portal/AxisGroup.xml)内，见变量`UpperPosLimit`, `LowerPosLimit`；硬限位光电开关对应的通道号，见变量`PositiveHardBit`，`NegativeHardBit`；扭矩限幅见变量`TorPdoMax`，该值的具体含义为指令力矩最大值相对于额定力矩的比例，单位为千分比；
2. 具体实现在`Driver`类内的[IsSoftLimitExceeded](./SlaveControlProgram/SlaveControlProgram/MotionControl/Axis/Driver.cpp#L308)，[IsHardLimitExceeded](./SlaveControlProgram/SlaveControlProgram/MotionControl/Axis/Driver.cpp#L326)函数，会判断当前位置是否越过软硬限位；[SendTargetTorque](./SlaveControlProgram/SlaveControlProgram/MotionControl/Axis/Driver.cpp#L68)函数，在发送扭矩指令前会将指令扭矩限制在上下限幅内；
3. `ControllerBase`类的[SafetyCheck](./SlaveControlProgram/SlaveControlProgram/MotionControl/Controller/ControllerBase.cpp#L130)函数会在每个周期的`Input`更新之后，判断当前位置是否超过限位，从而判断系统是否要跳入`eLimitViolation`状态，进而停止电机的运动；同时也会判断驱动器中是否产生C1D类的报错，从而判断系统是否要跳入`eFault`状态；

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

