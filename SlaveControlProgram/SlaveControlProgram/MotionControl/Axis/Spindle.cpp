#pragma once
#pragma hdrstop

#include "Spindle.h"

CSpindle::CSpindle() 
{
	
}

CSpindle::~CSpindle()
{
	m_Axis.~CAxis();
}

void CSpindle::MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters)
{
	m_Axis.MapParameters(&_pInputs->SpindleInput, &_pOutputs->SpindleOutput, &_pParameters->SpindleDriverParam, &_pParameters->SpindleInterpolationParam);
}

bool CSpindle::PostConstruction()
{
	return m_Axis.PostConstruction();
}

bool CSpindle::Initialize()
{
	return m_Axis.Initialize();
}

void CSpindle::Input()
{
	m_Axis.Input();

	m_FdbPos = m_Axis.m_FdbPos;
	m_FdbVel = m_Axis.m_FdbVel;
	m_FdbTor = m_Axis.m_FdbTor;
	m_CurrentOpMode = m_Axis.m_ActualOpMode;
}

void CSpindle::Output()
{
	m_Axis.Output();
}

bool CSpindle::Enable()
{
	return m_Axis.Enable();
}

bool CSpindle::Disable()
{
	return m_Axis.Disable();
}

void CSpindle::HoldPosition()
{
	m_Axis.HoldPosition();
}

void CSpindle::Move(double _cmd, OpMode _mode)
{
	m_Axis.Move(_cmd, _mode, kInterpolated, true);
}

void CSpindle::SetZeroPoint()
{

}