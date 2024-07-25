#pragma once
#include "Axis.h"

//#include "CommonBase.h"

// Dimensions should be  the same as those defined in tmc file and PLC GlobalVars
constexpr int kAxisNum = 4;
constexpr int kJointNum = 1;

class CControllerBase //: public CCommonBase
{
	
public:
	CControllerBase();
	~CControllerBase();

// Cyclic Run
public:
	void Run(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs);

private:
	Axis::CAxis AxisInst[kAxisNum][kJointNum];
	Axis::AxisParam m_AxisParam[kAxisNum][kJointNum];

	void Input(MotionControlInputs* pInputs);
	void Output(MotionControlOutputs* pOutputs);

	// State Machine
	void StateControl(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs);
	SystemState mCurrSysState;
	SystemState mSysStateMachine;

	bool m_bInit = false;
	bool StateInitialize();
	void StateStandby();
	void StateMoving(MotionControlInputs* pInputs);
	void StateHandwheel(MotionControlInputs* pInputs);
	void StateEmergencyStop();
	void StateTest(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs);

// Motor Set and Control
private:
	bool GetMotionCtrlParam(MotionControlInputs* pInputs);
	bool SetMotorParam();
	bool MotorServoOn();
	bool MotorServoOff();

private:
	double m_InitPos[kAxisNum][kJointNum] = { };
	double m_HandwheelInitPos[kAxisNum][kJointNum] = { };
			
	double m_CmdPos[kAxisNum][kJointNum] = { };
	double m_CmdVel[kAxisNum][kJointNum] = { };
	double m_CmdAcc[kAxisNum][kJointNum] = { };
	double m_CmdTor[kAxisNum][kJointNum] = { };
			
	double m_FdbPos[kAxisNum][kJointNum] = { };
	double m_FdbVel[kAxisNum][kJointNum] = { };
	double m_FdbAcc[kAxisNum][kJointNum] = { };
	double m_FdbTor[kAxisNum][kJointNum] = { };

// Test functions
private:
	void TestMoveSin(MotionControlInputs* pInputs);
	void TestCSP(double _length);
	void TestCSV(double _vel);
	void TestCST(double _tor);
	void TestBrake();
	void TestDefault();
	double KmatrixController(double fdbPos, double fdbVel, double fdbAcc, double cmdPos, double cmdVel, double cmdAcc, double Kp, double Kv, double Ka);
};