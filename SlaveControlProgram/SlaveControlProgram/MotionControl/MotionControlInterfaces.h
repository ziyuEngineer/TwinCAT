///////////////////////////////////////////////////////////////////////////////
// MotionControlInterfaces.h

#pragma once

#include "TcInterfaces.h"
#include "TcIoInterfaces.h"
#include "TcRtInterfaces.h"
#include "MotionControlServices.h"

///<AutoGeneratedContent id="Interfaces">
#if !defined(_TC_TYPE_6B2564DD_368D_427F_B764_2303628B4EFB_INCLUDED_)
#define _TC_TYPE_6B2564DD_368D_427F_B764_2303628B4EFB_INCLUDED_
struct __declspec(novtable) IMotionControlInterface : public ITcUnknown
{
	virtual HRESULT TCOMAPI ExecuteCommandsContinuously(ULONG n_min_data_start, ULONG& buffer_size, OpMode op_mode)=0;
	virtual HRESULT TCOMAPI EnterRecoveryState(bool is_entered)=0;
	virtual HRESULT TCOMAPI RequestStop()=0;
	virtual HRESULT TCOMAPI EnterFaultState()=0;
	virtual HRESULT TCOMAPI ExitFaultState()=0;
	virtual HRESULT TCOMAPI EnterStandbyState()=0;
	virtual HRESULT TCOMAPI EnterDisabledState()=0;
	virtual HRESULT TCOMAPI ScopeInitialize()=0;
	virtual HRESULT TCOMAPI ScopeStartRecord()=0;
	virtual HRESULT TCOMAPI ScopeStopRecord()=0;
	virtual HRESULT TCOMAPI ScopeExportData()=0;
	virtual HRESULT TCOMAPI ExecuteAxisGroupPositioning(PositioningAxis& axis, PositioningTarget& target)=0;
	virtual HRESULT TCOMAPI ExecuteToolSwitching(SHORT tool_num)=0;
};
_TCOM_SMARTPTR_TYPEDEF(IMotionControlInterface, IID_IMotionControlInterface);
#endif // !defined(_TC_TYPE_6B2564DD_368D_427F_B764_2303628B4EFB_INCLUDED_)

#if !defined(_TC_TYPE_DDC37991_4B80_49F3_A44A_196D30549B96_INCLUDED_)
#define _TC_TYPE_DDC37991_4B80_49F3_A44A_196D30549B96_INCLUDED_
struct __declspec(novtable) ISoEProcess : public ITcUnknown
{
	virtual HRESULT TCOMAPI mInitialize()=0;
	virtual HRESULT TCOMAPI mReset(bool bExecute)=0;
	virtual HRESULT TCOMAPI mSoEWriteVelocityLoopKp(SHORT axis, ULONG value)=0;
	virtual HRESULT TCOMAPI mSoEWritePositionLoopKv(SHORT axis, USHORT value)=0;
	virtual HRESULT TCOMAPI mSoEWriteVelocityLoopTn(SHORT axis, USHORT value)=0;
	virtual HRESULT TCOMAPI mSoEReadVelocityLoopKp(SHORT axis, ULONG& value)=0;
	virtual HRESULT TCOMAPI mSoEReadPositionLoopKv(SHORT axis, USHORT& value)=0;
	virtual HRESULT TCOMAPI mSoEReadVelocityLoopTn(SHORT axis, USHORT& value)=0;
};
_TCOM_SMARTPTR_TYPEDEF(ISoEProcess, IID_ISoEProcess);
#endif // !defined(_TC_TYPE_DDC37991_4B80_49F3_A44A_196D30549B96_INCLUDED_)

#if !defined(_TC_TYPE_8912121D_6A99_4443_9716_8F2A85D3B99E_INCLUDED_)
#define _TC_TYPE_8912121D_6A99_4443_9716_8F2A85D3B99E_INCLUDED_
struct __declspec(novtable) IPanelProcess : public ITcUnknown
{
	virtual HRESULT TCOMAPI mInitialize()=0;
	virtual HRESULT TCOMAPI mFaultReset()=0;
	virtual HRESULT TCOMAPI mInput()=0;
	virtual HRESULT TCOMAPI mOutput()=0;
	virtual HRESULT TCOMAPI mHandwheelReset()=0;
	virtual HRESULT TCOMAPI mDeselectAxis()=0;
	virtual HRESULT TCOMAPI mPopServoButton()=0;
};
_TCOM_SMARTPTR_TYPEDEF(IPanelProcess, IID_IPanelProcess);
#endif // !defined(_TC_TYPE_8912121D_6A99_4443_9716_8F2A85D3B99E_INCLUDED_)

#if !defined(_TC_TYPE_A5B190D2_781F_49A4_9D2B_66AD8519040D_INCLUDED_)
#define _TC_TYPE_A5B190D2_781F_49A4_9D2B_66AD8519040D_INCLUDED_
struct __declspec(novtable) ISpindleInterface : public ITcUnknown
{
	virtual HRESULT TCOMAPI RequestRotating(SpindleRot RotData)=0;
	virtual HRESULT TCOMAPI RequestStop()=0;
	virtual HRESULT TCOMAPI SetVelLimit(SpindleVelLimit VelLimit)=0;
	virtual HRESULT TCOMAPI RequestPositioning(SpindlePosition PositionData)=0;
	virtual HRESULT TCOMAPI ReportCurrentState(SpindleState& curr_state)=0;
	virtual HRESULT TCOMAPI RequestSpindleEnable()=0;
	virtual HRESULT TCOMAPI IsSpindleEnable(bool& is_enable)=0;
	virtual HRESULT TCOMAPI IsSpindleMoving(bool& is_moving)=0;
	virtual HRESULT TCOMAPI RequestFault()=0;
	virtual HRESULT TCOMAPI RequestReset()=0;
	virtual HRESULT TCOMAPI RequestSpindleDisable()=0;
};
_TCOM_SMARTPTR_TYPEDEF(ISpindleInterface, IID_ISpindleInterface);
#endif // !defined(_TC_TYPE_A5B190D2_781F_49A4_9D2B_66AD8519040D_INCLUDED_)

#if !defined(_TC_TYPE_125C4828_6162_4C51_984A_E44EFC60C49E_INCLUDED_)
#define _TC_TYPE_125C4828_6162_4C51_984A_E44EFC60C49E_INCLUDED_
struct __declspec(novtable) IAxisGroupInterface : public ITcUnknown
{
	virtual HRESULT TCOMAPI ReportCurrentState(AxisGroupState& curr_state)=0;
	virtual HRESULT TCOMAPI RequestEnterHandwheel()=0;
	virtual HRESULT TCOMAPI RequestQuitHandwheel()=0;
	virtual HRESULT TCOMAPI RequestContinuousMoving()=0;
	virtual HRESULT TCOMAPI RequestStop()=0;
	virtual HRESULT TCOMAPI RequestAxisGroupEnable()=0;
	virtual HRESULT TCOMAPI RequestAxisGroupDisable()=0;
	virtual HRESULT TCOMAPI RequestReset()=0;
	virtual HRESULT TCOMAPI RequestChangeOpMode(OpMode mode)=0;
	virtual HRESULT TCOMAPI IsAxisGroupStandby(bool& is_standby)=0;
	virtual HRESULT TCOMAPI IsAxisGroupMoving(bool& is_moving)=0;
	virtual HRESULT TCOMAPI IsAxisGroupOpModeSwitched(bool& is_switched)=0;
	virtual HRESULT TCOMAPI IsAxisGroupDisabled(bool& is_disabled)=0;
	virtual HRESULT TCOMAPI IsAxisGroupManualMoving(bool& is_manual)=0;
	virtual HRESULT TCOMAPI RequestFault()=0;
	virtual HRESULT TCOMAPI SetAxisVelLoopKp(SHORT axis, double value)=0;
	virtual HRESULT TCOMAPI SetAxisVelLoopTn(SHORT axis, double value)=0;
	virtual HRESULT TCOMAPI SetAxisPosLoopKv(SHORT axis, double value)=0;
	virtual HRESULT TCOMAPI GetAxisVelLoopKp(SHORT axis, double& value)=0;
	virtual HRESULT TCOMAPI GetAxisVelLoopTn(SHORT axis, double& value)=0;
	virtual HRESULT TCOMAPI GetAxisPosLoopKv(SHORT axis, double& value)=0;
	virtual HRESULT TCOMAPI RequestMovingToTargetPos(bool* axis, double* pos)=0;
};
_TCOM_SMARTPTR_TYPEDEF(IAxisGroupInterface, IID_IAxisGroupInterface);
#endif // !defined(_TC_TYPE_125C4828_6162_4C51_984A_E44EFC60C49E_INCLUDED_)

#if !defined(_TC_TYPE_DFA7B551_502B_4085_88E7_A88921AF739F_INCLUDED_)
#define _TC_TYPE_DFA7B551_502B_4085_88E7_A88921AF739F_INCLUDED_
struct __declspec(novtable) ISafetyInterface : public ITcUnknown
{
	virtual HRESULT TCOMAPI EnterRecoveryState()=0;
	virtual HRESULT TCOMAPI LeaveRecoveryState()=0;
};
_TCOM_SMARTPTR_TYPEDEF(ISafetyInterface, IID_ISafetyInterface);
#endif // !defined(_TC_TYPE_DFA7B551_502B_4085_88E7_A88921AF739F_INCLUDED_)

#if !defined(_TC_TYPE_ECB4C8CD_4DFF_4EA0_97A5_D5D0C67E798D_INCLUDED_)
#define _TC_TYPE_ECB4C8CD_4DFF_4EA0_97A5_D5D0C67E798D_INCLUDED_
struct __declspec(novtable) IScopeProcess : public ITcUnknown
{
	virtual HRESULT TCOMAPI mInitialize()=0;
	virtual HRESULT TCOMAPI mStartRecord()=0;
	virtual HRESULT TCOMAPI mStopRecord()=0;
	virtual HRESULT TCOMAPI mExportData()=0;
};
_TCOM_SMARTPTR_TYPEDEF(IScopeProcess, IID_IScopeProcess);
#endif // !defined(_TC_TYPE_ECB4C8CD_4DFF_4EA0_97A5_D5D0C67E798D_INCLUDED_)

///</AutoGeneratedContent>
