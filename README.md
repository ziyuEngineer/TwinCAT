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

提供C++模块调用的RPC接口；

### mInput

1. 接收部分硬件的输入信号, 如控制面板等；
2. xxxxxx

#### Data Area -- PLC Inputs

| 变量名 | 类型 | 大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| MAIN.rpc_PanelProcess.pInput | ARRAY [0..5] OF USINT | 6.0 | PANEL_Inputs | 获取面板输入通道的值 | \ |
| MAIN.rpc_PanelProcess.handwheelCurrPos | USINT | 1.0 | TEACH_HANDWHEEL . PANEL_Inputs . Panel . Device 3 (EtherCAT) . Devices | 获取手轮当前位置 | \ |
| MAIN.rpc_SoEProcess.stPlcDriveRef | ARRAY [0..4,0..1] OF Tc2_EtherCAT.ST_PlcDriveRef | 100.0 | (netId/port/Chn).AdsAddr.InfoData.(ZY/Gantry) | 访问驱动器 | \ |
| \ | \ | \ | \ | \ | \ |

### mOutput

1. xxxxxxx

#### Data Area -- PLC Outputs

| 变量名 | 类型 | 大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| MAIN.rpc_SoEProcess.objID | OTCID | 4.0 | OidSoEProcess . Inputs . MotionControl_Obj3 (ModuleAxisGroup) . MotionControl | SoE清错功能块的object ID | \ |
| MAIN.rpc_PanelProcess.objID | OTCID | 4.0 | OidPanelProcess . Inputs . MotionControl_Obj1 (MotionControl) . MotionControl | 面板处理功能块的object ID | \ |
| MAIN.rpc_PanelProcess.pOutput | ARRAY [0..5] OF USINT | 6.0 | PANEL_Outputs  | 给面板输出通道发送值 | \ |
| MAIN.rpc_PanelProcess.panelInfoOutput | PanelInfo | 64.0 | PanelInformation.Inputs.MotionControl_Obj1 (MotionControl).MotionControl  | 给C++端发送面板信息，如按钮状态，手轮行程等 | \ |
| \ | \ | \ | \ | \ | \ |

## MotionControl Project Brief

主模块状态机的运行，指令信号的处理(包括面板信号读取以及RingBuffer指令)；

### Input

1. 接收PLC模块传递的解析后的面板信号；
2. xxxxxx

#### Data Area -- MotionControl Inputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| Commandinput | ARRAY [0..1023] OF FullCommand | 778240.0 | 上位机写入 | 加工指令 | \ |
| CommandWriteIndex | UDINT | 4.0 | 上位机写入 | 下一个要写入的指令位置 | \ |
| PanelInformation | PanelInfo | 64.0 | MAIN.rpc_PanelProcess.panelInfoOutput.PlcTask Outputs.Portal Instance.Portal | 接收面板按钮的状态信息等 | \ |
| OidPanelProcess | OTCID | 4.0 | MAIN.rpc_PanelProcess.objID.PlcTask Outputs.Portal Instance.Portal | 初始化RPC | \ |

### StateControl

见[:arrow_right:](https://www.processon.com/diagraming/66b97fb9ce68f62ecf3af744)

### Output

1. 更新主模块状态机，供上位机读取；
2. xxxxxx

#### Data Area -- MotionControl Outputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| CommandReadIndex | UDINT | 4.0 | 上位机读取 | 下一个要写入的指令位置 | \ |
| MainState | SystemState | 2.0 | 上位机读取 | 主模块当前状态 | \ |
| ManualMovingCommand | 自定义结构体 | 48.0 | ManualMovingCommand.Inputs.MotionControl_Obj3 (ModuleAxisGroup).MotionControl | 手摇状态所需信息 | \ |
| ContinuousMovingCommand | FullCommand | 760.0 | ContinuousMovingCommand.Inputs.MotionControl_Obj3 (ModuleAxisGroup).MotionControl | 加工指令 | \ |

## ModuleSpindle Project Brief

主轴状态机的运行，主轴电机的控制；

### ModuleSpindle - Input

1. 接收主轴电机信号；
2. xxxxxx

#### Data Area -- ModuleSpindle Inputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| SpindleInput | DriverInput | 24.0 | 主轴电机的Input通道 | \ | \ |
| SpindleTestInputs | ARRAY [0..19] OF LREAL | 160.0 | \ | 测试用 | \ |

### ModuleSpindle - Output

1. 给主轴电机发送指令；
2. xxxxxx

#### Data Area -- ModuleSpindle Outputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| SpindleOutput | DriverOutput | 20.0 | 主轴电机的Output通道 | \ | \ |
| SpindleState | SpindleState | 2.0 | 上位机读取 | 主轴当前状态机 | \ |
| SpindleInfo | AxisInformation | 48.0 | 上位机读取 | 主轴当前信息 | \ |

## ModuleAxisGroup Project Brief

加工轴状态机的运行，加工轴电机的控制；

### ModuleAxisGroup - Input

1. 接收加工轴电机信号；；
2. xxxxxx

#### Data Area -- ModuleAxisGroup Inputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| OidSoEProcess | OTCID | 4.0 | MAIN.rpc_SoEProcess.objID.PlcTask Outputs.Portal Instance.Portal | 初始化SoE功能块RPC | \ |
| AxisInputs | ARRAY [0..9] OF DriverInput | 240.0 | 加工轴电机的Input通道 | \ | \ |
| ContinuousMovingCommand | FullCommand | 760.0 | ContinuousMovingCommand.Outputs.MotionControl_Obj1 (MotionControl).MotionControl | 连续加工指令 | \ |
| ManualMovingCommand | 自定义结构体 | 48.0 | ManualMovingCommand.Outputs.MotionControl_Obj1 (MotionControl).MotionControl | 手摇运动指令 | \ |

### ModuleAxisGroup - Output

1. 给加工轴电机发送指令；
2. xxxxxx

#### Data Area -- ModuleAxisGroup Outputs

| 变量名 | 类型 |  大小 | Linked To | 用途 | 备注 |
| :----: | :----: | :----: | :----: | :----: | :----: |
| AxisOutputs | ARRAY [0..9] OF DriverOutput | 200.0 | 加工轴电机的Output通道 | \ | \ |
| AxisGroupState | AxisGroupState | 2.0 | 上位机读取 | 加工轴当前状态机 | \ |
| AxisInfo | 自定义结构体 | 248.0 | 上位机读取 | 加工轴当前信息 | \ |

### 主模块及子系统状态机对应关系
| MainState | AxisGroupState |  SpindleState |
| :----: | :----: | :----: |
| Disabled | Disabled | Disabled |


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

