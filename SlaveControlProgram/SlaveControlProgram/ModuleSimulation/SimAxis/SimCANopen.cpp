#include "../TcPch.h"
#pragma hdrstop
#include "SimCANopen.h"

void CSimCANopen::MapParameters(DriverInput* sim_output, DriverOutput* sim_input, const MotionControlInfo* driver_param, const double cycle_time)
{
	m_SimOutput = sim_output;
	m_SimInput = sim_input;
	m_DriverParam = driver_param;

	m_CycleTime = cycle_time;
}

void CSimCANopen::Initialize()
{
	m_StatusWord = DriverStateGeneral::eNotReadyToSwitchOn;
	m_FdbPosVal = m_DriverParam->AbsZeroPos;
	m_FdbVelVal = 0;
	m_FdbTorVal = 0;
	m_ActualOpMode = 0;
	m_EffectivePosCmd = m_DriverParam->AbsZeroPos;
	m_EffectiveVelCmd = 0;
	m_EffectiveTorCmd = 0;
}

void CSimCANopen::Receive()
{
	m_ControlWord = m_SimInput->ControlWord;

	m_TargetPos = m_SimInput->TargetPosition;
	m_TargetVel = m_SimInput->TargetVelocity;
	m_TargetTor = m_SimInput->TargetTorque;

	m_OperationMode = m_SimInput->OperationMode;

	m_AdditiveTor = m_SimInput->AdditiveTorque;
	m_AdditivePos = m_SimInput->AdditivePosition;
	m_AdditiveVel = m_SimInput->AdditiveVelocity;
}

void CSimCANopen::Transmit()
{
	m_SimOutput->StatusWord = m_StatusWord;

	m_SimOutput->ActualAbsPos = m_FdbPosVal;
	m_SimOutput->ActualVel = m_FdbVelVal;
	m_SimOutput->ActualTor = m_FdbTorVal;

	m_SimOutput->ActualOpMode = m_ActualOpMode;

	m_SimOutput->EffectivePositionCmd = m_EffectivePosCmd;
	m_SimOutput->EffectiveVelocityCmd = m_EffectiveVelCmd;
	m_SimOutput->EffectiveTorqueCmd = m_EffectiveTorCmd;
}

void CSimCANopen::ReactionsToReceivedCommands()
{
	ReactionToControlWord();
	ReactionToTargetCommand();
}

void CSimCANopen::ReactionToControlWord()
{
	switch (m_StatusWord)
	{
	case DriverStateGeneral::eNotReadyToSwitchOn:
		m_StatusWord = DriverStateGeneral::eSwitchOnDisabled;
		break;

	case DriverStateGeneral::eSwitchOnDisabled:
		if (m_ControlWord == ControlWordGeneral::eShutdown)
		{
			m_StatusWord = DriverStateGeneral::eReadyToSwitchOn;
		}
		break;

	case DriverStateGeneral::eReadyToSwitchOn:
		if (m_ControlWord == ControlWordGeneral::eSwitchOn)
		{
			m_StatusWord = DriverStateGeneral::eSwitchedOn;
		}
		break;

	case DriverStateGeneral::eSwitchedOn:
		if (m_ControlWord == ControlWordGeneral::eEnableOperation)
		{
			m_StatusWord = DriverStateGeneral::eOperationEnabled;
		}
		break;

	case DriverStateGeneral::eOperationEnabled:
		if (m_ControlWord == ControlWordGeneral::eDisableOperation)
		{
			m_StatusWord = DriverStateGeneral::eSwitchedOn;
			m_ActualOpMode = 0;
		}
		// Operation mode can only be switched in enabled state;
		m_ActualOpMode = m_OperationMode;
		break;
	}
}

void CSimCANopen::ReactionToTargetCommand()
{
	switch (m_ActualOpMode)
	{
	case OpMode::CSP: 
		ReactionToPosCommand();
		break;

	case OpMode::CST: 
		ReactionToTorCommand();
		break;

	case OpMode::CSV:
		ReactionToVelCommand();
		break;

	default:
		// No response to commands since driver is disabled
		break;
	}
}

void CSimCANopen::ReactionToPosCommand()
{
	m_FdbVelVal = static_cast<long>((m_TargetPos - m_FdbPosVal) / m_CycleTime);
	m_EffectiveVelCmd = m_FdbVelVal;

	m_FdbPosVal = m_TargetPos;
	m_EffectivePosCmd = m_TargetPos;

	// TODO: update torque
}

void CSimCANopen::ReactionToVelCommand()
{
	m_FdbPosVal += static_cast<long>(m_FdbVelVal * m_CycleTime);
	m_EffectivePosCmd = m_FdbPosVal;

	m_FdbVelVal = m_TargetVel;
	m_EffectiveVelCmd = m_TargetVel;

	// TODO: update torque
}

void CSimCANopen::ReactionToTorCommand()
{
	m_FdbTorVal = m_TargetTor;
	m_EffectiveTorCmd = m_TargetTor;

	// TODO: update velocity and position
}