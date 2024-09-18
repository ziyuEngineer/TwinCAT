#pragma once

#include "Axis.h"

class CAxisGroup
{
public:
	CAxisGroup();
	~CAxisGroup();

//private:
//	CAxis m_Axes[kMaxAxisNum][kMaxMotorNumPerAxis];

public:
	CAxis m_Axes[kMaxAxisNum][kMaxMotorNumPerAxis];
	void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters);
	bool PostConstruction();

	bool Initialize();
	void Input();
	void Output();
	bool Disable();
	bool Enable();
	void HoldPosition();
	void Move(MotionCommand _cmd);
	void Handwheel(PanelInfo _panel);
	void SingleAxisMove(AxisNum _axis_index, double _cmd, OpMode _mode);
	void ReturnToZeroPoint(AxisNum _axis_index);

	SystemState SafetyCheck();

	double m_FdbPos[kMaxAxisNum];
	double m_FdbVel[kMaxAxisNum];
	double m_FdbTor[kMaxAxisNum];
	OpMode m_CurrentOpMode[kMaxAxisNum];
	double m_GantryDeviation;
	double m_InitPos_Handwheel[kMaxAxisNum];
};