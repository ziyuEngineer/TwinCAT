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
	SHORT m_DriverNumPerAxis[kMaxAxisNum] = { 0 }; // INT type in tmc

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
	void AxisGroupPositioning(const bool axis_enabled[5], const double target[5]);
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

	HRESULT SetSingleAxisVelLoopKp(SHORT axis, double value);
	HRESULT SetSingleAxisVelLoopTn(SHORT axis, double value);
	HRESULT SetSingleAxisPosLoopKv(SHORT axis, double value);
	HRESULT GetSingleAxisVelLoopKp(SHORT axis, double& value);
	HRESULT GetSingleAxisVelLoopTn(SHORT axis, double& value);
	HRESULT GetSingleAxisPosLoopKv(SHORT axis, double& value);
	void ResetTuningError();

	double GetSingleAxisPosition(int axis_index);
	void StopComputingTuningError();

private:
	// For auto tunning
	bool m_IsTuningStart = false;
	int m_TuneAxisNum = 0;
	double m_VelError[kMaxAxisNum] = { 0 };
	double m_PosError[kMaxAxisNum] = { 0 };
	double m_LastEffectiveVelCmd;
	double m_LastEffectivePosCmd;
	bool AutoTuneInitialize();
	void UpdateLastEffectiveCommand();
	void ComputeTuningError();
	void ComputeAccumulatedVelocityError();
	void ComputeAccumulatedPositionError();

	// For safety torque check
	double m_LastEffectiveTorCmd[kMaxAxisNum * kMaxMotorNumPerAxis] = { 0 };
};