#pragma once
#include "Axis.h"
#include "Biquad.h"
#include <unordered_map>
#include <unordered_set>

//#include "CommonBase.h"

// Dimensions should be  the same as those defined in tmc file and PLC GlobalVars
constexpr int kAxisNum = 2;
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
	void RequestStateChangeTo(SystemState _request_state);
	SystemState m_CurrSysState;

	// 定义状态转换规则
	std::unordered_map<SystemState, std::unordered_set<SystemState>> permittedTransitionsIn;

	bool m_bInit = false;
	bool StateInitialize();
	void StateStandby();
	void StateMoving(MotionControlInputs* pInputs);
	void StateHandwheel(MotionControlInputs* pInputs);
	void StateLimitViolation();
	void StateTest(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs);

	// Safety Module
	void SafetyCheck();

// Motor Set and Control
private:
	bool GetMotionCtrlParam(MotionControlInputs* pInputs);
	bool SetMotorParam();
	bool MotorEnable();
	bool MotorDisable();
	void MotorHoldPosition();

private:
	double m_InitPos[kAxisNum][kJointNum] = { };
	double m_HandwheelInitPos[kAxisNum][kJointNum] = { };
	double m_MovingInitPos[kAxisNum][kJointNum] = { };
			
	double m_CmdPos[kAxisNum][kJointNum] = { };
	double m_CmdVel[kAxisNum][kJointNum] = { };
	double m_CmdAcc[kAxisNum][kJointNum] = { };
	double m_CmdTor[kAxisNum][kJointNum] = { };
			
	double m_FdbPos[kAxisNum][kJointNum] = { };
	double m_FdbVel[kAxisNum][kJointNum] = { };
	double m_FdbAcc[kAxisNum][kJointNum] = { };
	double m_FdbTor[kAxisNum][kJointNum] = { };
	OpMode m_ActualOpMode[kAxisNum][kJointNum] = { };

	Biquad m_FdbPosFilter[kAxisNum][kJointNum];
	Biquad m_FdbVelFilter[kAxisNum][kJointNum];

// Test functions
private:
	void TestMoveSin(MotionControlInputs* pInputs);
	void TestMoveSinCSP(double _A, double _freq);
	void TestCSP(double _pos, int _axis);
	void TestSinCSV(double _A, double _freq);
	void TestCSV(double _vel);
	void TestSinCST(double _A, double _freq);
	void TestCST(double _tor);
	void TestBrake();
	void TestDefault();
	double KmatrixController(double fdbPos, double fdbVel, double fdbAcc, double cmdPos, double cmdVel, double cmdAcc, double Kp, double Kv, double Ka);

	double m_SimTime = 0.000;
	double m_TestFdbPos[kAxisNum][kJointNum] = { };
};