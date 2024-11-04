#include "../TcPch.h"
#pragma hdrstop
#include "CommandController.h"

IPanelProcess* CCommandController::m_pPanelProcess = nullptr;
ISpindleInterface* CCommandController::m_pSpindleInterface = nullptr;
IAxisGroupInterface* CCommandController::m_pAxisGroupInterface = nullptr;

CCommandController::CCommandController()
{
	m_MainState = SystemState::eIdle;
}

CCommandController::~CCommandController()
{

}

void CCommandController::MapParameters(MotionControlInputs* inputs, MotionControlOutputs* outputs)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_CommandManager.MapParameters(inputs, outputs);
}

void CCommandController::Input()
{
	PanelProcess();
}

void CCommandController::Output()
{
	m_pOutputs->MainState = m_MainState;
}

void CCommandController::PanelProcess()
{
	m_pPanelProcess->mInput(); // Parse input signal
	m_pPanelProcess->mOutput(); // Update button status
}

bool CCommandController::IsReadyToStandby()
{
	bool is_axisgroup_standby;
	bool is_spindle_enable;
	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	m_pSpindleInterface->IsSpindleEnable(is_spindle_enable);

	return is_axisgroup_standby && is_spindle_enable;
}

bool CCommandController::IsMovingFinished()
{
	// Currently, only a stop motion instruction is placed at the end of the ringbuffer, 
	// which applies to AxisGroup and Spindle.
	return (m_CommandManager.GetCurrentCommand().metaData.CommandType == CommandType::eSignalFinalized);
}

bool CCommandController::RingBufferDispatchCommand()
{
	if (m_CommandManager.GetCommandFromBuffer())
	{
		FullCommand temp_command = m_CommandManager.GetCurrentCommand();
		switch(temp_command.metaData.CommandType)
		{
		case CommandType::eMotion:
			m_pOutputs->ContinuousMovingCommand = temp_command;
			break;

		case CommandType::eOther:
			// TODO : rewrite as switch-case in future if more functions are introduced
			if (temp_command.otherCommand.CommandFunction == CommandFunction::eSpindle)
			{
				SpindleRot spindle_rot;
				spindle_rot.TargetVel = m_CommandManager.m_Command.otherCommand.Data;
				m_pSpindleInterface->RequestRotating(spindle_rot);
			}
			break;
		}
		return true;
	}
	return false;
}

void CCommandController::QuitMachining()
{
	m_CommandManager.Reset();
	RequestSpindleStop();
	RequestAxisGroupStop();
}

bool CCommandController::IsServoOnPressed()
{
	return m_pInputs->PanelInformation.ServoOn;
}

bool CCommandController::IsManualModeSelected()
{
	return (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum != 0);
}

bool CCommandController::IsExecutionPressed()
{
	return m_pInputs->PanelInformation.Execute;
}

int CCommandController::IsSpindleMovingPressed()
{
	return m_pInputs->PanelInformation.Spindle_Cmd;
}

void CCommandController::UpdateManualMovingCommand()
{
	m_pOutputs->ManualMovingCommand.SelectedAxis = m_pInputs->PanelInformation.Handwheel_EnabledAxisNum;
	memcpy(&m_pOutputs->ManualMovingCommand.MovingCommand, &m_pInputs->PanelInformation.Handwheel_dPos, sizeof(double[5]));
}

void CCommandController::RingBufferInput()
{
	m_CommandManager.Input();
}

void CCommandController::RingBufferOutput()
{
	m_CommandManager.Output();
}

int CCommandController::GetCurrentRingBufferSize()
{
	return m_CommandManager.GetCurrentBufferSize();
}

int CCommandController::GetMaxBufferSize()
{
	return m_CommandManager.GetMaxBufferSize();
}

bool CCommandController::IsReadyToMove()
{
	bool is_axisgroup_standby;
	bool is_spindle_enable;
	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	m_pSpindleInterface->IsSpindleEnable(is_spindle_enable);

	return is_axisgroup_standby && is_spindle_enable;
}

bool CCommandController::IsReadyToReceiveCmd()
{
	return (m_pInputs->CommandWriteIndex == 0);
}

void CCommandController::RequestSpindleRotate(double vel)
{
	SpindleRot spindle_rot;
	spindle_rot.TargetVel = vel;
	m_pSpindleInterface->RequestRotating(spindle_rot);
}

void CCommandController::RequestSpindleStop()
{
	m_pSpindleInterface->RequestStop();
}

void CCommandController::RequestSpindleEnable()
{
	m_pSpindleInterface->RequestSpindleEnable();
}

void CCommandController::RequestSpindleDisable()
{
	//m_pSpindleInterface->RequestSpindleDisable();
}

void CCommandController::RequestAxisGroupServoOn()
{
	m_pAxisGroupInterface->RequestAxisGroupEnable();
}

void CCommandController::RequestAxisGroupServoOff()
{
	m_pAxisGroupInterface->RequestAxisGroupDisable();
}

void CCommandController::RequestAxisGroupChangeOpMode(OpMode mode)
{
	m_pAxisGroupInterface->RequestChangeOpMode(mode);
}

void CCommandController::RequestAxisGroupMove()
{
	m_pAxisGroupInterface->RequestContinuousMoving();
}

void CCommandController::RequestAxisGroupStop()
{
	m_pAxisGroupInterface->RequestStop();
}

void CCommandController::RequestAxisGroupEnterHandwheel()
{
	m_pAxisGroupInterface->RequestEnterHandwheel();
}

void CCommandController::RequestAxisGroupQuitHandwheel()
{
	m_pPanelProcess->mHandwheelReset();
	memset(&m_pOutputs->ManualMovingCommand, 0, sizeof(m_pOutputs->ManualMovingCommand));
	m_pAxisGroupInterface->RequestQuitHandwheel();
}

void CCommandController::RequestAxisGroupResetError()
{
	m_pAxisGroupInterface->RequestReset();
}

void CCommandController::RequestAxisGroupRecoveryState(bool is_entered)
{
	m_pAxisGroupInterface->RequestRecovery(is_entered);
	if (!is_entered)
	{
		m_pPanelProcess->mHandwheelReset();
	}
}

bool CCommandController::IsAxisGroupOpModeChanged()
{
	bool is_changed = false;
	m_pAxisGroupInterface->IsAxisGroupOpModeSwitched(is_changed);
	return is_changed;
}

bool CCommandController::IsAxisGroupDisabled()
{
	bool is_disabled = false;
	m_pAxisGroupInterface->IsAxisGroupDisabled(is_disabled);
	return is_disabled;
}