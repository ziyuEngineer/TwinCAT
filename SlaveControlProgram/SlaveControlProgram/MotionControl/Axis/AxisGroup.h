#pragma once

#include "Axis.h"

class CAxisGroup
{
public:
	CAxisGroup();
	~CAxisGroup();

private:
	int m_ActualAxisNum = 0;
	SHORT m_DriverNumPerAxis[5] = {0}; // INT type in tmc

public:
	CAxis m_Axes[kMaxAxisNum][kMaxMotorNumPerAxis];
	void MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, const ModuleAxisGroupParameter* parameters);
	bool PostConstruction();

	bool Initialize();
	void Input();
	void Output();
	bool Disable();
	bool Enable();
	bool IsEnabled();
	bool IsDisabled();
	void HoldPosition();
	void StandStill();
	void Move(FullCommand cmd);
	void Handwheel(int axis_selected, double cmd[5]);
	void SingleAxisMove(AxisOrder axis_index, double cmd, OpMode mode);
	void SingleAxisMove(AxisOrder axis_index, FullCommand cmd);
	void Positioning(const bool axis_enabled[5], const double cmd[5]);
	void ReturnToZeroPoint(AxisOrder axis_index);
	void ResetInterpolator(OpMode mode);
	void SwitchOpMode(OpMode mode);
	bool IsOpModeSwitched();

	void ClearError();
	void QuickStop();

	double m_GantryDeviation;
	double m_InitPos_Handwheel[kMaxAxisNum];
};