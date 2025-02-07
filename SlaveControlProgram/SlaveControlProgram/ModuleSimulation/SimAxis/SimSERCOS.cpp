#include "../TcPch.h"
#pragma hdrstop
#include "SimSERCOS.h"

void CSimSERCOS::MapParameters(DriverInput* sim_output, DriverOutput* sim_input, const MotionControlInfo* driver_param, const double cycle_time)
{
	m_SimOutput = sim_output;
	m_SimInput = sim_input;
	m_DriverParam = driver_param;

	m_CycleTime = cycle_time;
}

void CSimSERCOS::Initialize()
{
	m_StatusWord = DriverState::eNotReadyForPowerUp;
	m_FdbPosVal = m_DriverParam->AbsZeroPos;
	m_FdbVelVal = 0;
	m_FdbTorVal = 0;
	m_EffectivePosCmd = m_DriverParam->AbsZeroPos;
	m_EffectiveVelCmd = 0;
	m_EffectiveTorCmd = 0;
}

void CSimSERCOS::Receive()
{
	m_ControlWord = m_SimInput->ControlWord;

	m_TargetPos = m_SimInput->TargetPosition;
	m_TargetVel = m_SimInput->TargetVelocity;
	m_TargetTor = m_SimInput->TargetTorque;

	m_AdditiveTor = m_SimInput->AdditiveTorque;
	m_AdditivePos = m_SimInput->AdditivePosition;
	m_AdditiveVel = m_SimInput->AdditiveVelocity;
}

void CSimSERCOS::Transmit()
{
	m_SimOutput->StatusWord = m_StatusWord;

	m_SimOutput->ActualAbsPos = m_FdbPosVal;
	m_SimOutput->ActualVel = m_FdbVelVal;
	m_SimOutput->ActualTor = m_FdbTorVal;

	m_SimOutput->EffectivePositionCmd = m_EffectivePosCmd;
	m_SimOutput->EffectiveVelocityCmd = m_EffectiveVelCmd;
	m_SimOutput->EffectiveTorqueCmd = m_EffectiveTorCmd;
}

void CSimSERCOS::ReactionsToReceivedCommands()
{
	ReactionToControlWord();
	ReactionToTargetCommand();
}

void CSimSERCOS::ReactionToControlWord()
{
	switch (m_StatusWord)
	{
	case DriverState::eNotReadyForPowerUp:
		m_StatusWord = DriverState::eLogicReadyForPowerOn;
		break;

	case DriverState::eLogicReadyForPowerOn:
		if (m_ControlWord == MasterControlWord::eControlUnitSync)
		{
			m_StatusWord = DriverState::ePowerStageLocked;
		}
		break;

	case DriverState::ePowerStageLocked:
	case DriverState::eReadyToOperate:
		HandlePowerTransitions();
		break;

	case DriverState::eShutDownError:
		m_StatusWord = DriverState::ePowerStageLocked;
		break;
	}
}

void CSimSERCOS::HandlePowerTransitions()
{
	switch (m_ControlWord)
	{
	case MasterControlWord::eDriveOff:
		m_StatusWord = DriverState::ePowerStageLocked;
		break;
	case MasterControlWord::eDriveOn_CSP:
	case MasterControlWord::eDriveOn_CSP + MasterControlWord::eControlUnitSync:
		m_StatusWord = DriverState::eReadyToOperate;
		break;
	case MasterControlWord::eDriveOn_CST:
	case MasterControlWord::eDriveOn_CST + MasterControlWord::eControlUnitSync:
		m_StatusWord = (DriverState::eReadyToOperate | (1 << 8));
		break;
	case MasterControlWord::eDriveOn_CSV:
	case MasterControlWord::eDriveOn_CSV + MasterControlWord::eControlUnitSync:
		m_StatusWord = (DriverState::eReadyToOperate | (1 << 9));
		break;
	default:
		break;
	}
}

void CSimSERCOS::ReactionToTargetCommand()
{
	CommandSafetyCheck();

	switch (m_StatusWord)
	{
	case DriverState::eReadyToOperate:				// Primary operation mode - CSP
		ReactionToPosCommand();
		break;

	case (DriverState::eReadyToOperate | (1 << 8)):	// Secondary operation mode1 - CST
		ReactionToTorCommand();
		break;

	case (DriverState::eReadyToOperate | (1 << 9)):	// Secondary operation mode2 - CSV
		ReactionToVelCommand();
		break;

	case DriverState::eNotReadyForPowerUp:
	case DriverState::eLogicReadyForPowerOn:
	case DriverState::ePowerStageLocked:
	case DriverState::eShutDownError:
		// No response to commands since driver is disabled
		break;
	}
}

void CSimSERCOS::ReactionToPosCommand()
{
	m_FdbVelVal = static_cast<long>((m_TargetPos - m_FdbPosVal) / m_CycleTime);
	m_EffectiveVelCmd = m_FdbVelVal;

	m_FdbPosVal = m_TargetPos;
	m_EffectivePosCmd = m_TargetPos;

	// TODO: update torque
}

void CSimSERCOS::ReactionToVelCommand()
{
	m_FdbPosVal += static_cast<long>(m_FdbVelVal * m_CycleTime);
	m_EffectivePosCmd = m_FdbPosVal;

	m_FdbVelVal = m_TargetVel;
	m_EffectiveVelCmd = m_TargetVel;

	// TODO: update torque
}

void CSimSERCOS::ReactionToTorCommand()
{
	m_FdbTorVal = m_TargetTor;
	m_EffectiveTorCmd = m_TargetTor;

	// TODO: update velocity and position
}

void CSimSERCOS::CommandSafetyCheck()
{
	if (m_StatusWord == DriverState::eReadyToOperate)
	{
		if (fabs_(m_FdbPosVal - m_TargetPos) > kMaxPosDeviation)
		{
			m_StatusWord = DriverState::eShutDownError;
		}
	}
}