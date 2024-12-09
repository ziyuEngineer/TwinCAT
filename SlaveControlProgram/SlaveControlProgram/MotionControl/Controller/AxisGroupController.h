#pragma once
#include "AxisGroup.h"

class CAxisGroupController
{
public:
	CAxisGroupController();
	~CAxisGroupController();
	
// Initialize
public:
	void MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, const ModuleAxisGroupParameter* parameters);
	bool PostConstruction();
	static ISoEProcess* m_pSoEProcess;

private:
	ModuleAxisGroupInputs* m_pInputs = nullptr;
	ModuleAxisGroupOutputs* m_pOutputs = nullptr;

	CAxisGroup m_AxisGroup;

	int m_ActualAxisNum = 0;
	int m_ActualDriverNum = 0;
	SHORT m_DriverNumPerAxis[5] = { 0 }; // INT type in tmc

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
	//void AxisGroupGetCurrentPos();
	void AxisGroupStandStill();
	void AxisGroupHandwheel();
	void AxisGroupLimitViolation();
	void AxisGroupFault();
	bool AxisGroupEnable();
	bool AxisGroupDisable();
	bool IsAxisGroupEnabled();
	bool IsAxisGroupDisabled();
	void AxisGroupResetInterpolator(OpMode mode);
	void AxisGroupSwitchOpMode(OpMode mode);
	bool IsAxisGroupOpModeSwitched();

	void NotifyPlcResetSoE(bool is_executed);
	void AxisGroupClearError();

private:
	// Test variables
	double m_SimTime = 0.0;
	double m_TestInitPos[kMaxAxisNum];
	double m_SimTimeSpindle = 0.0;
};