#pragma once

#include "Axis.h"

class CSpindle
{
public:
	CSpindle();
	~CSpindle();

//private:
//	CAxis m_Axis;

public:
	CAxis m_Axis;
	void MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters);
	bool PostConstruction();

	bool Initialize();
	void Input();
	void Output();

	bool Enable();
	bool Disable();

	void HoldPosition();
	void Move(double _cmd, OpMode _mode);
	void SetZeroPoint();

	double m_FdbPos;
	double m_FdbVel;
	double m_FdbTor;
	OpMode m_CurrentOpMode;
};