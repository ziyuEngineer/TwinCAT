#include "../TcPch.h"
#pragma hdrstop
#include "MainController.h"

IPanelProcess* CMainController::m_pPanelProcess = nullptr;
ISpindleInterface* CMainController::m_pSpindleInterface = nullptr;
IAxisGroupInterface* CMainController::m_pAxisGroupInterface = nullptr;
CTcTrace* CMainController::m_Trace = nullptr;

CMainController::CMainController()
{
	m_MainState = SystemState::eIdle;
}

CMainController::~CMainController()
{

}

void CMainController::MapParameters(MotionControlInputs* inputs, MotionControlOutputs* outputs)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
	m_CommandManager.MapParameters(inputs, outputs);
}

void CMainController::Input()
{
	PanelProcess();

	// MatrixAssign();
}

void CMainController::Output()
{
	m_pOutputs->MainState = m_MainState;

	// MatrixTest();
}

void CMainController::PanelProcess()
{
	m_pPanelProcess->mInput(); // Parse input signal
	m_pPanelProcess->mOutput(); // Update button status
}

bool CMainController::IsReadyToStandby()
{
	bool is_axisgroup_standby;
	bool is_spindle_enable;
	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	m_pSpindleInterface->IsSpindleEnable(is_spindle_enable);

	return is_axisgroup_standby && is_spindle_enable;
}

bool CMainController::IsMovingFinished()
{
	// Currently, only a stop motion instruction is placed at the end of the ringbuffer, 
	// which applies to AxisGroup and Spindle.
	return (m_CommandManager.GetCurrentCommand().otherCommand.CommandFunction == CommandFunction::eM30);
}

bool CMainController::RingBufferDispatchCommand()
{
	if (m_CommandManager.GetCommandFromBuffer())
	{
		FullCommand temp_command = m_CommandManager.GetCurrentCommand();
		
		switch(temp_command.metaData.CommandType)
		{
		case CommandType::eMotion:
		case CommandType::eSingleAxisX:
		case CommandType::eSingleAxisY:
		case CommandType::eSingleAxisZ:
		case CommandType::eSingleAxisA:
		case CommandType::eSingleAxisC:
			m_pOutputs->ContinuousMovingCommand = temp_command;
			break;

		case CommandType::eOther:
			// TODO : rewrite as switch-case in future if more functions are introduced
			if (temp_command.otherCommand.CommandFunction == CommandFunction::eSpindle)
			{
				SpindleRot spindle_rot{};
				spindle_rot.TargetVel = m_CommandManager.m_Command.otherCommand.Data;
				m_pSpindleInterface->RequestRotating(spindle_rot);
			}
			break;
		}
		
		return true;
	}
	return false;
}

void CMainController::EmptyRingBuffer()
{
	m_CommandManager.Reset();
}

void CMainController::ClearLocalCommand()
{
	memset(&m_pOutputs->ContinuousMovingCommand, 0, sizeof(FullCommand));
}

void CMainController::QuitMachiningNormally()
{
	ClearLocalCommand();
	EmptyRingBuffer();
	RequestSpindleStop();
	RequestAxisGroupStop();
}

void CMainController::QuitMachiningWithWarning()
{
	ClearLocalCommand();
	EmptyRingBuffer();
	RequestSpindleStop();
	RequestAxisGroupStop();
	m_Trace->Log(tlError, FNAMEA "Level tlError : Machining has stopped because there are no more commands.");
}

bool CMainController::IsServoOnPressed()
{
	return m_pInputs->PanelInformation.ServoOn;
}

bool CMainController::IsManualModeSelected()
{
	return (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum != 0);
}

bool CMainController::IsExecutionPressed()
{
	return m_pInputs->PanelInformation.Execute;
}

int CMainController::IsSpindleMovingPressed()
{
	return m_pInputs->PanelInformation.Spindle_Cmd;
}

void CMainController::UpdateManualMovingCommand()
{
	m_pOutputs->ManualMovingCommand.SelectedAxis = m_pInputs->PanelInformation.Handwheel_EnabledAxisNum;
	memcpy(&m_pOutputs->ManualMovingCommand.MovingCommand, &m_pInputs->PanelInformation.Handwheel_dPos, sizeof(double[5]));
}

void CMainController::UpdateManualCommandInRecoveryState(ModuleError dir, AxisOrder axis)
{
	int index = static_cast<int>(axis);
	if (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum == index)
	{
		m_pOutputs->ManualMovingCommand.SelectedAxis = m_pInputs->PanelInformation.Handwheel_EnabledAxisNum;

		if (dir == ModuleError::PosUpperOver)
		{
			m_pOutputs->ManualMovingCommand.MovingCommand[index - 1] = m_pInputs->PanelInformation.Handwheel_dPos[index - 1] <= 0.0 ? m_pInputs->PanelInformation.Handwheel_dPos[index - 1] : 0.0;
		}
		else if (dir == ModuleError::PosLowerOver)
		{
			m_pOutputs->ManualMovingCommand.MovingCommand[index - 1] = m_pInputs->PanelInformation.Handwheel_dPos[index - 1] >= 0.0 ? m_pInputs->PanelInformation.Handwheel_dPos[index - 1] : 0.0;
		}
	}
}

bool CMainController::IsResetPressed()
{
	return m_pInputs->PanelInformation.Reset;
}

bool CMainController::PanelReset()
{
	m_pPanelProcess->mFaultReset();
	return true;
}

bool CMainController::DeselectAxisAutomatically()
{
	m_pPanelProcess->mDeselectAxis();
	return true;
}

bool CMainController::DeselectServoButtonAutomatically()
{
	m_pPanelProcess->mPopServoButton();
	return true;
}

void CMainController::RingBufferInput()
{
	m_CommandManager.Input();
}

void CMainController::RingBufferOutput()
{
	m_CommandManager.Output();
}

ULONG CMainController::GetCurrentRingBufferSize()
{
	return m_CommandManager.GetCurrentBufferSize();
}

bool CMainController::IsLatestCommandStop()
{
	FullCommand last_cmd;
	m_CommandManager.GetLatestCommand(last_cmd);
	return (last_cmd.metaData.CommandType == CommandType::eOther) && (last_cmd.otherCommand.CommandFunction == CommandFunction::eM30);
}

ULONG CMainController::GetMaxBufferSize()
{
	return m_CommandManager.GetMaxBufferSize();
}

bool CMainController::IsReadyToMove()
{
	bool is_axisgroup_standby;
	bool is_axisgroup_manual;
	bool is_spindle_enable;
	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	m_pAxisGroupInterface->IsAxisGroupManualMoving(is_axisgroup_manual);
	m_pSpindleInterface->IsSpindleEnable(is_spindle_enable);

	return (is_axisgroup_standby || is_axisgroup_manual) && is_spindle_enable;
}

bool CMainController::IsReadyToReceiveCmd()
{
	return (m_pInputs->CommandWriteIndex == 0);
}

void CMainController::RequestSpindleRotate(double vel)
{
	SpindleRot spindle_rot{};
	spindle_rot.TargetVel = vel;
	m_pSpindleInterface->RequestRotating(spindle_rot);
}

void CMainController::RequestSpindleStop()
{
	m_pSpindleInterface->RequestStop();
}

void CMainController::RequestSpindleEnable()
{
	m_pSpindleInterface->RequestSpindleEnable();
}

void CMainController::RequestSpindleDisable()
{
	m_pSpindleInterface->RequestSpindleDisable();
}

void CMainController::RequestSpindleFault()
{
	m_pSpindleInterface->RequestFault();
}

void CMainController::RequestSpindleResetError()
{
	m_pSpindleInterface->RequestReset();
}

void CMainController::RequestAxisGroupServoOn()
{
	m_pAxisGroupInterface->RequestAxisGroupEnable();
}

void CMainController::RequestAxisGroupServoOff()
{
	m_pAxisGroupInterface->RequestAxisGroupDisable();
}

void CMainController::RequestAxisGroupChangeOpMode(OpMode mode)
{
	m_pAxisGroupInterface->RequestChangeOpMode(mode);
}

void CMainController::RequestAxisGroupMove()
{
	m_pAxisGroupInterface->RequestContinuousMoving();
}

void CMainController::RequestAxisGroupStop()
{
	m_pAxisGroupInterface->RequestStop();
}

void CMainController::RequestAxisGroupEnterHandwheel()
{
	m_pAxisGroupInterface->RequestEnterHandwheel();
}

void CMainController::RequestAxisGroupQuitHandwheel()
{
	m_pPanelProcess->mHandwheelReset();
	memset(&m_pOutputs->ManualMovingCommand, 0, sizeof(m_pOutputs->ManualMovingCommand));
	m_pAxisGroupInterface->RequestQuitHandwheel();
}

void CMainController::RequestAxisGroupFault()
{
	m_pAxisGroupInterface->RequestFault();
}

void CMainController::RequestAxisGroupResetError()
{
	m_pAxisGroupInterface->RequestReset();
}

bool CMainController::IsAxisGroupOpModeChanged()
{
	bool is_changed = false;
	m_pAxisGroupInterface->IsAxisGroupOpModeSwitched(is_changed);
	return is_changed;
}

bool CMainController::IsAxisGroupDisabled()
{
	bool is_disabled = false;
	m_pAxisGroupInterface->IsAxisGroupDisabled(is_disabled);
	return is_disabled;
}

bool CMainController::IsAxisGroupReadyForPositioning()
{
	bool is_axisgroup_standby;

	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	
	return is_axisgroup_standby;
}

void CMainController::RequestAxisGroupPositioning(bool moving_axis[5], double target[5])
{
	m_pAxisGroupInterface->RequestMovingToTargetPos(moving_axis, target);
}

ULONG CMainController::GetErrorCode()
{
	return m_pInputs->ErrorCode;
}

bool CMainController::IsSystemNormal()
{
	return m_pInputs->ErrorCode == 0;
}

bool CMainController::MatrixAssign()
{
	for (int i = 0; i < 50; i++)
	{
		for (int j = 0; j < 50; j++)
		{
			m_MatrixA(i, j) = 0.123;
			//m_MatrixB(i, j) = 0.321;
		}
		m_VectorB[i] = 0.321;
	}
	return true;
}

void CMainController::MatrixTest()
{
	Vector<50> result;
	result = m_MatrixA * m_VectorB;
}