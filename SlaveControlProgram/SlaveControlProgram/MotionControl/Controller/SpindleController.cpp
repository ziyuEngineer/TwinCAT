#pragma once
#pragma hdrstop

#include "SpindleController.h"

CSpindleController::CSpindleController()
{
	m_SpindleState = SpindleState::eSpindleIdle;
}


CSpindleController::~CSpindleController()
{

}

void CSpindleController::MapParameters(ModuleSpindleInputs* inputs, ModuleSpindleOutputs* outputs, ModuleSpindleParameter* parameters)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_Spindle.MapParameters(inputs, outputs, parameters);
}

bool CSpindleController::PostConstruction()
{
	return m_Spindle.PostConstruction();
}

/**
 * @brief Process input signals from hardware and data from PLC module
 *
 */
void CSpindleController::Input()
{
	m_Spindle.Input();
}

/**
 * @brief Send commands or data to output channels of hardware and PLC module
 *
 */
void CSpindleController::Output()
{
	m_pOutputs->StateSpindle = m_SpindleState;

	m_Spindle.Output();

	m_pOutputs->SpindleInfo.CurrentPos = m_Spindle.m_FdbPos;
	m_pOutputs->SpindleInfo.CurrentVel = m_Spindle.m_FdbVel;
	m_pOutputs->SpindleInfo.CurrentTor = m_Spindle.m_FdbTor;
	m_pOutputs->SpindleInfo.CurrentMode = m_Spindle.m_CurrentOpMode;
	m_pOutputs->SpindleInfo.CurrentStatus = static_cast<int>(m_Spindle.m_CurrentDriverStatus);

	m_pOutputs->SpindleInfo.CommandPos = m_Spindle.m_Axis.m_CmdPos;
	m_pOutputs->SpindleInfo.CommandVel = m_Spindle.m_Axis.m_CmdVel;
	m_pOutputs->SpindleInfo.CommandTor = m_Spindle.m_Axis.m_CmdTor;

}

SpindleState CSpindleController::GetCurrentState()
{
	return m_SpindleState;
}

bool CSpindleController::SpindleInitialize()
{
	return m_Spindle.Initialize();
}

void CSpindleController::SpindleStandby()
{
	m_Spindle.HoldPosition(); // or set velocity to zero
}

void CSpindleController::SpindleMoving(SpindlePosition spindle_cmd)
{
	m_Spindle.Move(spindle_cmd);
}

void CSpindleController::SpindleMoving(SpindleRot spindle_cmd)
{
	m_Spindle.Move(spindle_cmd);
}

void CSpindleController::SpindleBrake()
{
	m_Spindle.Move(0.0, OpMode::CSV);
}

void CSpindleController::SpindleStandStill()
{
	m_Spindle.StandStill();
}

void CSpindleController::SpindleFault()
{
	SpindleDisable();
}

bool CSpindleController::SpindleEnable()
{
	return m_Spindle.Enable();
}

bool CSpindleController::SpindleDisable()
{
	return m_Spindle.Disable();
}

bool CSpindleController::IsSpindleEnabled()
{
	return m_Spindle.IsEnabled();
}

bool CSpindleController::IsSpindleMotionless()
{
	return fabs_(m_Spindle.m_FdbVel) == 0.0;
}

void CSpindleController::SpindleSwitchOpMode(OpMode mode)
{
	m_Spindle.SwitchOpMode(mode);
}

bool CSpindleController::IsSpindleOpModeSwitched()
{
	return m_Spindle.IsOpModeSwitched();
}

void CSpindleController::SpindleResetInterpolator(OpMode mode)
{
	m_Spindle.ResetInterpolator(mode);
}

void CSpindleController::SpindleClearError()
{
	m_Spindle.ClearError();
}

void CSpindleController::SetSpindleVelLimit(SpindleVelLimit vel_limit)
{
	m_SpindleVelLimit = vel_limit;
}
