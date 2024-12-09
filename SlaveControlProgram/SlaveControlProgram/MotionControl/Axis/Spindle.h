#pragma once

#include "Axis.h"

class CSpindle
{
public:
	CSpindle();
	~CSpindle();

public:
	CAxis m_Axis;
	void MapParameters(ModuleSpindleInputs* inputs, ModuleSpindleOutputs* outputs, ModuleSpindleParameter* parameters);
	bool PostConstruction();

	bool Initialize();
	void Input();
	void Output();

	bool Enable();
	bool Disable();
	bool IsEnabled();
	void SwitchOpMode(OpMode mode);
	bool IsOpModeSwitched();

	void HoldPosition();
	void StandStill();
	void Move(double cmd, OpMode mode);
	void Move(SpindlePosition cmd);
	void Move(SpindleRot cmd);
	void ResetInterpolator(OpMode mode);
	void ClearError();
	void QuickStop();

	double m_FdbPos;
	double m_FdbVel;
	double m_FdbTor;
	OpMode m_CurrentOpMode;
	DriverStatus m_CurrentDriverStatus;

	void SetZeroPoint();
	double m_ZeroPoint;
	bool m_IsZeroPointSet = false;
};