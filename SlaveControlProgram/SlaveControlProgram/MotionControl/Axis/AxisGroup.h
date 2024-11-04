#pragma once

#include "Axis.h"

class CAxisGroup
{
public:
	CAxisGroup();
	~CAxisGroup();

public:
	CAxis m_Axes[kMaxAxisNum][kMaxMotorNumPerAxis];
	void MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, ModuleAxisGroupParameter* parameters);
	bool PostConstruction();

	bool Initialize();
	void Input();
	void Output();
	bool Disable();
	bool Enable();
	bool IsEnabled();
	void HoldPosition();
	void StandStill();
	void Move(FullCommand cmd);
	void Handwheel(int axis_selected, double cmd[5]);
	void SingleAxisMove(AxisNum axis_index, double cmd, OpMode mode);
	void ReturnToZeroPoint(AxisNum axis_index);
	void ResetInterpolator(OpMode mode);
	void SwitchOpMode(OpMode mode);
	bool IsOpModeSwitched();

	AxisGroupState SafetyCheck();

	double m_FdbPos[kMaxAxisNum];
	double m_FdbVel[kMaxAxisNum];
	double m_FdbTor[kMaxAxisNum];
	OpMode m_CurrentOpMode[kMaxAxisNum];
	double m_GantryDeviation;
	double m_InitPos_Handwheel[kMaxAxisNum];
};