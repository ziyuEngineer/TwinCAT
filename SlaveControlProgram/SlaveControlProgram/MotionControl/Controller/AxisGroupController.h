#pragma once
#include "AxisGroup.h"

class CAxisGroupController
{
public:
	CAxisGroupController();
	~CAxisGroupController();
	
// Initialize
public:
	void MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, ModuleAxisGroupParameter* parameters);
	bool PostConstruction();
	static ISoEProcess* m_pSoEProcess;

private:
	ModuleAxisGroupInputs* m_pInputs = nullptr;
	ModuleAxisGroupOutputs* m_pOutputs = nullptr;

	CAxisGroup m_AxisGroup;

public:
	// Cyclic Run
	void Input(); // Update Inputs from hardware or host machine
	void Output(); // Update Outputs to hardware or host machine

	// AxisGroup State Machine
	AxisGroupState m_AxisGroupState;
	AxisGroupState GetCurrentState();
	bool AxisGroupInitialize();
	void AxisGroupStandby();
	void AxisGroupMoving();
	void AxisGroupStandStill();
	void AxisGroupHandwheel();
	void AxisGroupLimitViolation();
	void AxisGroupFault();
	void AxisGroupRecovery();
	bool AxisGroupEnable();
	bool AxisGroupDisable();
	bool IsAxisGroupEnabled();
	void AxisGroupResetInterpolator(OpMode mode);
	void AxisGroupSwitchOpMode(OpMode mode);
	bool IsAxisGroupOpModeSwitched();
	// Safety Module
	AxisGroupState AxisGroupSafetyCheck();

	// Local state switch trigger
	void NotifyPlcResetSoE(bool is_executed);

private:
	// Test variables
	double m_SimTime = 0.0;
	double m_TestInitPos[kMaxAxisNum];
	double m_SimTimeSpindle = 0.0;
};