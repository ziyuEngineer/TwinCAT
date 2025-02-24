#include "../TcPch.h"
#pragma hdrstop
#include "AxisGroupController.h"

ISoEProcess* CAxisGroupController::m_pSoEProcess = nullptr;
ITcEventLogger* CAxisGroupController::m_pEventLogger = nullptr;
ITcMessage* CAxisGroupController::m_pMessage = nullptr;

CAxisGroupController::CAxisGroupController()
{
	m_AxisGroupState = AxisGroupState::eAxisGroupIdle;
}


CAxisGroupController::~CAxisGroupController()
{

}

void CAxisGroupController::MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, const ModuleAxisGroupParameter* parameters)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_pParameters = parameters;
	m_AxisGroup.MapParameters(inputs, outputs, parameters);

	m_ActualAxisNum = parameters->ActualAxisNum;
	m_ActualDriverNum = parameters->TotalDriverNum;
	memcpy(m_DriverNumPerAxis, parameters->DriverNumPerAxis, sizeof(m_DriverNumPerAxis));

	int driver_num = 0;
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_DriverAxisMap[driver_num] = static_cast<AxisOrder>(i + 1);
			driver_num++;
		}
	}
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
	m_pOutputs->StateAxisGroup = m_AxisGroupState;

	m_AxisGroup.Output();

	int driver_num = 0;
	for (int j = 0; j < m_ActualAxisNum; j++)
	{
		for (int k = 0; k < m_DriverNumPerAxis[j]; k++)
		{
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CurrentPos = m_AxisGroup.m_Axes[j][k].m_FdbPos;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CurrentVel = m_AxisGroup.m_Axes[j][k].m_FdbVel;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CurrentTor = m_AxisGroup.m_Axes[j][k].m_FdbTor;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CurrentMode = m_AxisGroup.m_Axes[j][k].m_ActualOpMode;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CurrentStatus = static_cast<int>(m_AxisGroup.m_Axes[j][k].m_CurrentDriverStatus);
															
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CommandPos = m_AxisGroup.m_Axes[j][k].m_CmdPos;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CommandVel = m_AxisGroup.m_Axes[j][k].m_CmdVel;
			m_pOutputs->AxisGroupInfo.SingleAxisInformation[driver_num].CommandTor = m_AxisGroup.m_Axes[j][k].m_CmdTor;

			//Update torque diagnostic information
			m_pOutputs->AxisGroupTorqueInfo.TorqueCommandDeviation[driver_num] = fabs_(m_LastEffectiveTorCmd[driver_num] - m_AxisGroup.m_Axes[j][k].m_EffectiveTorCmd);
			m_pOutputs->AxisGroupTorqueInfo.TorqueFollowingError[driver_num] = fabs_(m_LastEffectiveTorCmd[driver_num] - m_AxisGroup.m_Axes[j][k].m_FdbTor);
			m_LastEffectiveTorCmd[driver_num] = m_AxisGroup.m_Axes[j][k].m_EffectiveTorCmd;
			driver_num++;
		}
	}

	m_pOutputs->AxisGroupInfo.GantryDeviation = m_AxisGroup.m_GantryDeviation;

	ComputeTuningError();

	AxisGroupPosLimitCheck();
}

AxisGroupState CAxisGroupController::GetCurrentState()
{
	return m_AxisGroupState;
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
 * @brief Action under moving state, execute commands received from main module
 */
void CAxisGroupController::AxisGroupMoving()
{
	if (static_cast<CommandType>(m_pInputs->ContinuousMovingCommand.metaData.CommandType) == CommandType::eMotion)
	{
		m_AxisGroup.Move(m_pInputs->ContinuousMovingCommand);
	}
	else if(m_pInputs->ContinuousMovingCommand.metaData.CommandType >= CommandType::eSingleAxisX)
	{
		if (!m_IsTuningStart)
		{
			m_IsTuningStart = AutoTuneInitialize();
		}

		AxisOrder moving_axis = static_cast<AxisOrder>(m_pInputs->ContinuousMovingCommand.metaData.CommandType - 9);
		m_AxisGroup.SingleAxisMove(moving_axis, m_pInputs->ContinuousMovingCommand);
	}
}

void CAxisGroupController::AxisGroupStandStill()
{
	m_AxisGroup.StandStill();
}

/**
 * @brief Action under handwheel state, add incremental positions calculated by main module to current positions
 */
void CAxisGroupController::AxisGroupHandwheel()
{
	ProcessHandwheelInfo();
	m_AxisGroup.Handwheel(m_pInputs->ManualMovingCommand.SelectedAxis, m_ManualMovingDistance);
}

void CAxisGroupController::AxisGroupPositioning(const bool axis_enabled[5], const double target[5])
{
	m_AxisGroup.Positioning(axis_enabled, target);
}

void CAxisGroupController::AxisGroupFault()
{
	AxisGroupDisable();
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

bool CAxisGroupController::IsAxisGroupDisabled()
{
	return m_AxisGroup.IsDisabled();
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

void CAxisGroupController::AxisGroupPosLimitCheck()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		int axis_index = m_DriverAxisMap.at(i) - 1;
		if (m_pOutputs->AxisGroupInfo.SingleAxisInformation[i].CurrentPos < m_pParameters->AxisPosLowerLimit[i])
		{
			m_IsPosLowerLimitTriggered[axis_index] = true;
			if (!m_IsLowerLimitEventSent[axis_index])
			{
				DispatchEventMessage(TcEvents::AxisGroupEvent::AxisGroupPosLowerOver.nEventId, m_DriverAxisMap.at(i));
				m_IsLowerLimitEventSent[axis_index] = true;
			}
		}
		else if (m_pOutputs->AxisGroupInfo.SingleAxisInformation[i].CurrentPos > m_pParameters->AxisPosUpperLimit[i])
		{
			m_IsPosUpperLimitTriggered[axis_index] = true;
			if (!m_IsUpperLimitEventSent[axis_index])
			{
				DispatchEventMessage(TcEvents::AxisGroupEvent::AxisGroupPosUpperOver.nEventId, m_DriverAxisMap.at(i));
				m_IsUpperLimitEventSent[axis_index] = true;
			}
		}
		else
		{
			m_IsPosUpperLimitTriggered[axis_index] = false;
			m_IsPosLowerLimitTriggered[axis_index] = false;
			m_IsUpperLimitEventSent[axis_index] = false;
			m_IsLowerLimitEventSent[axis_index] = false;
		}
	}
}

void CAxisGroupController::NotifyPlcResetSoE(bool is_executed)
{
	// Invoke plc method
	m_pSoEProcess->mReset(is_executed);
}

void CAxisGroupController::AxisGroupClearError()
{
	m_AxisGroup.ClearError();
}

HRESULT CAxisGroupController::SetSingleAxisVelLoopKp(SHORT axis, double value)
{
	HRESULT hr = S_FALSE;
	// Decimal point 3
	ULONG SoE_value = static_cast<ULONG>(value * 1000.0);
	hr = m_pSoEProcess->mSoEWriteVelocityLoopKp(axis, SoE_value);

	return hr;
}

HRESULT CAxisGroupController::SetSingleAxisVelLoopTn(SHORT axis, double value)
{
	HRESULT hr = S_FALSE;
	// Decimal point 1
	USHORT SoE_value = static_cast<USHORT>(value * 10.0);
	hr = m_pSoEProcess->mSoEWriteVelocityLoopTn(axis, SoE_value);

	return hr;
}

HRESULT CAxisGroupController::SetSingleAxisPosLoopKv(SHORT axis, double value)
{
	HRESULT hr = S_FALSE;
	// Decimal point 2
	USHORT SoE_value = static_cast<USHORT>(value * 100.0);
	hr = m_pSoEProcess->mSoEWritePositionLoopKv(axis, SoE_value);

	return hr;
}

HRESULT CAxisGroupController::GetSingleAxisVelLoopKp(SHORT axis, double& value)
{
	HRESULT hr = S_FALSE;
	ULONG SoE_value;

	hr = m_pSoEProcess->mSoEReadVelocityLoopKp(axis, SoE_value);
	value = static_cast<double>(SoE_value) / 1000.0;

	return hr;
}

HRESULT CAxisGroupController::GetSingleAxisVelLoopTn(SHORT axis, double& value)
{
	HRESULT hr = S_FALSE;
	USHORT SoE_value;

	hr = m_pSoEProcess->mSoEReadVelocityLoopTn(axis, SoE_value);
	value = static_cast<double>(SoE_value) / 10.0;

	return hr;
}

HRESULT CAxisGroupController::GetSingleAxisPosLoopKv(SHORT axis, double& value)
{
	HRESULT hr = S_FALSE;
	USHORT SoE_value;

	hr = m_pSoEProcess->mSoEReadPositionLoopKv(axis, SoE_value);
	value = static_cast<double>(SoE_value) / 100.0;

	return hr;
}

bool CAxisGroupController::AutoTuneInitialize()
{
	m_TuneAxisNum = static_cast<int>(m_pInputs->ContinuousMovingCommand.metaData.CommandType - 10);

	memset(&m_VelError, 0, sizeof(m_VelError));
	memset(&m_PosError, 0, sizeof(m_PosError));
	memset(&m_pOutputs->AxisGroupTuningInfo.TuningVelError, 0, sizeof(m_pOutputs->AxisGroupTuningInfo.TuningVelError));
	memset(&m_pOutputs->AxisGroupTuningInfo.TuningPosError, 0, sizeof(m_pOutputs->AxisGroupTuningInfo.TuningPosError));

	UpdateLastEffectiveCommand();
	return true;
}

void CAxisGroupController::UpdateLastEffectiveCommand()
{
	m_LastEffectiveVelCmd = m_AxisGroup.m_Axes[m_TuneAxisNum][0].m_EffectiveVelCmd;
	m_LastEffectivePosCmd = m_AxisGroup.m_Axes[m_TuneAxisNum][0].m_EffectivePosCmd;
}

void CAxisGroupController::ComputeTuningError()
{
	if (m_IsTuningStart)
	{
		ComputeAccumulatedVelocityError();
		ComputeAccumulatedPositionError();

		UpdateLastEffectiveCommand();
	}
	else
	{
		m_pOutputs->AxisGroupTuningInfo.TuningVelError[m_TuneAxisNum] = m_VelError[m_TuneAxisNum];
		m_pOutputs->AxisGroupTuningInfo.TuningPosError[m_TuneAxisNum] = m_PosError[m_TuneAxisNum];
	}
}

void CAxisGroupController::StopComputingTuningError()
{
	m_IsTuningStart = false;
}

void CAxisGroupController::ComputeAccumulatedVelocityError()
{
	m_VelError[m_TuneAxisNum] += fabs_(m_LastEffectiveVelCmd - m_AxisGroup.m_Axes[m_TuneAxisNum][0].m_FdbVel);
}

void CAxisGroupController::ComputeAccumulatedPositionError()
{
	m_PosError[m_TuneAxisNum] += fabs_(m_LastEffectivePosCmd - m_AxisGroup.m_Axes[m_TuneAxisNum][0].m_FdbPos);
}

void CAxisGroupController::ResetTuningError()
{
	m_IsTuningStart = false;
	memset(&m_VelError, 0, sizeof(m_VelError));
	memset(&m_PosError, 0, sizeof(m_PosError));
	memset(&m_pOutputs->AxisGroupTuningInfo.TuningVelError, 0, sizeof(m_pOutputs->AxisGroupTuningInfo.TuningVelError));
	memset(&m_pOutputs->AxisGroupTuningInfo.TuningPosError, 0, sizeof(m_pOutputs->AxisGroupTuningInfo.TuningPosError));
}

double CAxisGroupController::GetSingleAxisPosition(int axis_index)
{
	return m_AxisGroup.m_Axes[axis_index][0].m_FdbPos;
}

void CAxisGroupController::ProcessHandwheelInfo()
{
	int index = m_pInputs->ManualMovingCommand.SelectedAxis - 1;
	if (index >= 0) 
	{
		double dPos = m_pInputs->ManualMovingCommand.HandwheelPos - m_LastHandwheelPos;

		if (dPos > m_ImpossibleScaleChange)
		{
			dPos = m_HandwheelFullScale - dPos;
		}
		else if (dPos < -m_ImpossibleScaleChange)
		{
			dPos = m_HandwheelFullScale + dPos;
		}

		// Ignore dPos update when axis is out of soft position limit
		if (m_IsPosLowerLimitTriggered[index] && dPos < 0.0)
		{
			dPos = 0.0;
		}
		else if (m_IsPosUpperLimitTriggered[index] && dPos > 0.0)
		{
			dPos = 0.0;
		}
		m_ManualMovingDistance[index] = m_ManualMovingDistance[index] + m_pInputs->ManualMovingCommand.Ratio * dPos;
	}

	m_LastHandwheelPos = m_pInputs->ManualMovingCommand.HandwheelPos;
}

void CAxisGroupController::ClearHandwheelInfo()
{
	memset(&m_ManualMovingDistance, 0, sizeof(m_ManualMovingDistance));
}

void CAxisGroupController::DispatchEventMessage(ULONG event_id, AxisOrder axis_num)
{
	m_pEventLogger->CreateMessage(TcEvents::AxisGroupEvent::EventClass, event_id, TcEventSeverity::Info, &TcSourceInfo("AxisGroup Module"), &m_pMessage);
	char attribute[20];
	sprintf(attribute, "%d", static_cast<int>(axis_num));
	m_pMessage->SetJsonAttribute(attribute);
	m_pMessage->Send(0);
}
