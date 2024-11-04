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

void CSpindle::MapParameters(ModuleSpindleInputs* inputs, ModuleSpindleOutputs* outputs, ModuleSpindleParameter* parameters)
{
	m_Axis.MapParameters(&inputs->SpindleInput, &outputs->SpindleOutput, &parameters->SpindleDriverParam, &parameters->SpindleInterpolationParam);
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

bool CSpindle::IsEnabled()
{
	return m_Axis.IsEnabled();
}

void CSpindle::SwitchOpMode(OpMode mode)
{
	m_Axis.SwitchOperationMode(mode);
}

bool CSpindle::IsOpModeSwitched()
{
	return m_Axis.IsOpModeSwitched();
}

void CSpindle::HoldPosition()
{
	m_Axis.HoldPosition();
}

void CSpindle::StandStill()
{
	m_Axis.StandStill();
}

void CSpindle::Move(double cmd, OpMode mode)
{
	m_Axis.Move(cmd, mode, kInterpolated, true);
}

void CSpindle::Move(SpindlePosition cmd)
{
	m_Axis.Move(cmd.TargetPos, OpMode::CSP, kNotInterpolated, false);
}

void CSpindle::Move(SpindleRot cmd)
{
	m_Axis.Move(cmd.TargetVel, OpMode::CSV, kInterpolated, false);
}

void CSpindle::ResetInterpolator(OpMode mode)
{
	m_Axis.InterpolationReset(mode);
}

// Reserve for future use
void CSpindle::SetZeroPoint()
{
	if (!m_IsZeroPointSet)
	{
		m_ZeroPoint = static_cast<double>(static_cast<int>(m_FdbPos));
		m_IsZeroPointSet = true;
	}
}