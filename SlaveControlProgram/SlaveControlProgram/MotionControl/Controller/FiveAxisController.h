#pragma once
#include "Driver.h"

#include "AxisGroup.h"
#include "Spindle.h"
#include <unordered_map>
#include <unordered_set>

class CFiveAxisController
{
public:
	CFiveAxisController();
	~CFiveAxisController();
	
	// Cyclic Run
public:
	void Run();
	void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters);
	bool PostConstruction();

private:
	MotionControlInputs* m_pInputs = nullptr;
	MotionControlOutputs* m_pOutputs = nullptr;

	CAxisGroup m_AxisGroup;
	CSpindle m_Spindle;

	void Input();
	void Output();

	// State Machine
	void StateControl();
	SystemState m_CurrSysState;
	// 定义状态转换规则
	std::unordered_map<SystemState, std::unordered_set<SystemState>> permittedTransitionsIn;
	void RequestStateChange(SystemState* _current_state, SystemState _request_state);

	bool m_bInit = false;
	bool StateInitialize();
	void StateStandby();
	void StateMoving();
	void StateHandwheel();
	void StateLimitViolation();
	void StateFault();
	void StateTest();

	bool SystemEnable();
	bool SystemDisable();

	// Safety Module
	void SafetyCheck();

private:
	// Test variables
	double m_SimTime = 0.0;
	double m_TestInitPos[kMaxAxisNum];
};