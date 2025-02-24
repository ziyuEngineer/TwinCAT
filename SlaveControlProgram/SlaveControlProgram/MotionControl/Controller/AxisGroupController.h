#pragma once
#include "AxisGroup.h"
#include <unordered_map>

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

	// Event logger
	static ITcEventLogger* m_pEventLogger;
	static ITcMessage* m_pMessage;

private:
	ModuleAxisGroupInputs* m_pInputs = nullptr;
	ModuleAxisGroupOutputs* m_pOutputs = nullptr;
	const ModuleAxisGroupParameter* m_pParameters = nullptr;

	CAxisGroup m_AxisGroup;

	int m_ActualAxisNum = 0;
	int m_ActualDriverNum = 0;
	SHORT m_DriverNumPerAxis[kMaxAxisNum] = { 0 }; // INT type in tmc
	std::unordered_map<int, AxisOrder> m_DriverAxisMap;

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
	
	// Event Logger
	void DispatchEventMessage(ULONG event_id, AxisOrder axis_num);

	void ProcessHandwheelInfo();
	void ClearHandwheelInfo();

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

	// Handwheel process
	double m_HandwheelFullScale = 256.0;
	double m_ImpossibleScaleChange = 100.0;
	double m_LastHandwheelPos;
	double m_ManualMovingDistance[kMaxAxisNum] = { 0.0 };

	// AxisGroup internal check
	void AxisGroupPosLimitCheck();
	bool m_IsPosUpperLimitTriggered[kMaxAxisNum] = { false };
	bool m_IsPosLowerLimitTriggered[kMaxAxisNum] = { false };
	// The following flags are used to prevent sending too many duplicate event logs
	bool m_IsUpperLimitEventSent[kMaxAxisNum] = { false };
	bool m_IsLowerLimitEventSent[kMaxAxisNum] = { false };
};