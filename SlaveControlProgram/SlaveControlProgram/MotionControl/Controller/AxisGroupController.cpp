#include "../TcPch.h"
#pragma hdrstop
#include "AxisGroupController.h"

ISoEProcess* CAxisGroupController::m_pSoEProcess = nullptr;

CAxisGroupController::CAxisGroupController()
{
	m_AxisGroupState = AxisGroupState::eAxisGroupIdle;
}


CAxisGroupController::~CAxisGroupController()
{

}

void CAxisGroupController::MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, ModuleAxisGroupParameter* parameters)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_AxisGroup.MapParameters(inputs, outputs, parameters);
}

bool CAxisGroupController::PostConstruction()
{
	return m_AxisGroup.PostConstruction();
}

/**
 * @brief Process input signals from hardware and data from PLC module
 *
 */
void CAxisGroupController::Input()
{
	m_AxisGroup.Input();
}

/**
 * @brief Send commands or data to output channels of hardware and PLC module
 *
 */
void CAxisGroupController::Output()
{
	m_pOutputs->AxisGroupState = m_AxisGroupState;

	m_AxisGroup.Output();

	for (int i = 0; i < kActualAxisNum; i++)
	{
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentPos = m_AxisGroup.m_FdbPos[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentVel = m_AxisGroup.m_FdbVel[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentTor = m_AxisGroup.m_FdbTor[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentMode = m_AxisGroup.m_CurrentOpMode[i];

		m_pOutputs->AxisInfo.AxisStatus[i].CommandPos = m_AxisGroup.m_Axes[i][0].m_CmdPos;
		m_pOutputs->AxisInfo.AxisStatus[i].CommandVel = m_AxisGroup.m_Axes[i][0].m_CmdVel;
		m_pOutputs->AxisInfo.AxisStatus[i].CommandTor = m_AxisGroup.m_Axes[i][0].m_CmdTor;
	}

	m_pOutputs->AxisInfo.GantryDeviation = m_AxisGroup.m_GantryDeviation;

	// Locally Observe
	// m_pOutputs->TestAxisGroup = m_CommandManager.m_Command;

}

AxisGroupState CAxisGroupController::GetCurrentState()
{
	return m_AxisGroupState;
}


AxisGroupState CAxisGroupController::AxisGroupSafetyCheck()
{
	return m_AxisGroup.SafetyCheck();
}

/**
 * @brief Action under initialize state, set parameters of motors
 *
 * @return true
 * @return false
 */
bool CAxisGroupController::AxisGroupInitialize()
{
	return m_AxisGroup.Initialize();
}

/**
 * @brief Action under standby state, hold current positions
 *
 */
void CAxisGroupController::AxisGroupStandby()
{
	m_AxisGroup.HoldPosition();
}

/**
 * @brief Action under moving state, execute commands received from plc module
 */
void CAxisGroupController::AxisGroupMoving()
{
	m_AxisGroup.Move(m_pInputs->ContinuousMovingCommand);
}

void CAxisGroupController::AxisGroupStandStill()
{
	m_AxisGroup.StandStill();
}

/**
 * @brief Action under handwheel state, add incremental positions calculated by plc module to current positions
 */
void CAxisGroupController::AxisGroupHandwheel()
{
	m_AxisGroup.Handwheel(m_pInputs->ManualMovingCommand.SelectedAxis, m_pInputs->ManualMovingCommand.MovingCommand);
}

/**
 * @brief Action under limit violation state, disable motors when exceeding limits
 * 
 */
void CAxisGroupController::AxisGroupLimitViolation()
{
	//AxisGroupDisable();
	m_AxisGroup.HoldPosition();
}

void CAxisGroupController::AxisGroupFault()
{
	AxisGroupDisable();
}

void CAxisGroupController::AxisGroupRecovery()
{
	//m_AxisGroup.Handwheel(m_pInputs->PanelInformation);
}

bool CAxisGroupController::AxisGroupEnable()
{
	return m_AxisGroup.Enable();
}

bool CAxisGroupController::AxisGroupDisable()
{
	return m_AxisGroup.Disable();
}

bool CAxisGroupController::IsAxisGroupEnabled()
{
	return m_AxisGroup.IsEnabled();
}

void CAxisGroupController::AxisGroupResetInterpolator(OpMode mode)
{
	m_AxisGroup.ResetInterpolator(mode);
}

void CAxisGroupController::AxisGroupSwitchOpMode(OpMode mode)
{
	m_AxisGroup.SwitchOpMode(mode);
}

bool CAxisGroupController::IsAxisGroupOpModeSwitched()
{
	return m_AxisGroup.IsOpModeSwitched();
}

void CAxisGroupController::NotifyPlcResetSoE(bool is_executed)
{
	// Invoke plc method
	m_pSoEProcess->mReset(is_executed);
}

