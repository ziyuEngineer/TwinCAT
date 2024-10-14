#include "../TcPch.h"
#pragma hdrstop
#include "FiveAxisController.h"

ISoEProcess* CFiveAxisController::m_pSoEProcess = nullptr;
IPanelProcess* CFiveAxisController::m_pPanelProcess = nullptr;

CFiveAxisController::CFiveAxisController()
{
	m_AxisGroupState = SystemState::eIdle;
	m_SpindleState = SystemState::eIdle;
}


CFiveAxisController::~CFiveAxisController()
{

}

void CFiveAxisController::MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters)
{
	m_pInputs = _pInputs;
	m_pOutputs = _pOutputs;
	m_AxisGroup.MapParameters(_pInputs, _pOutputs, _pParameters);
	m_Spindle.MapParameters(_pInputs, _pOutputs, _pParameters);
	m_CommandManager.MapParameters(_pInputs, _pOutputs);
}

bool CFiveAxisController::PostConstruction()
{
	return m_AxisGroup.PostConstruction() && m_Spindle.PostConstruction();
}

/**
 * @brief Process input signals from hardware and data from PLC module
 *
 */
void CFiveAxisController::Input()
{
	m_CommandManager.Input();
	m_AxisGroup.Input();
	m_Spindle.Input();
	PanelProcess();
}

/**
 * @brief Send commands or data to output channels of hardware and PLC module
 *
 */
void CFiveAxisController::Output()
{
	m_pOutputs->SystemStateMachine.AxisGroupState = m_AxisGroupState;
	m_pOutputs->SystemStateMachine.SpindleState = m_SpindleState;

	m_AxisGroup.Output();
	m_Spindle.Output();
	m_CommandManager.Output();

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
	
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentPos = m_Spindle.m_FdbPos;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentVel = m_Spindle.m_FdbVel;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentTor = m_Spindle.m_FdbTor;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentMode = m_Spindle.m_CurrentOpMode;

	m_pOutputs->AxisInfo.AxisStatus[5].CommandPos = m_Spindle.m_Axis.m_CmdPos;
	m_pOutputs->AxisInfo.AxisStatus[5].CommandVel = m_Spindle.m_Axis.m_CmdVel;
	m_pOutputs->AxisInfo.AxisStatus[5].CommandTor = m_Spindle.m_Axis.m_CmdTor;

	// Locally Observe
	m_pOutputs->TestAxisGroup = m_CommandManager.m_Command;

	// Matrix test
	/*
	Mat5x5 _matrix1;
	Mat5x5 _matrix2;
	_matrix1.eye();
	_matrix2 = _matrix1;
	m_pOutputs->TestAxisGroup.Other_Command.Data = _matrix1(1, 1);
	m_pOutputs->TestSpindle.Other_Command.Data = _matrix2(4, 4);*/

	if (m_pInputs->mockButton[6])
	{
		MatrixTest();
	}
}

void CFiveAxisController::CommonModuleProcess()
{
	if (!m_bMovingStart)
	{
		m_bMovingStart = IsMovingStarted();
	}
	else
	{
		// Update local command to AxisGroup and Spindle
		GetCommand();
	}

	if (!m_bMovingFinish)
	{
		m_bMovingFinish = IsMovingFinished();
	}
	else
	{
		ResetCommand();
	}
}

SystemState CFiveAxisController::AxisGroupSafetyCheck()
{
	return m_AxisGroup.SafetyCheck();
}

/**
 * @brief Action under initialize state, set parameters of motors
 *
 * @return true
 * @return false
 */
bool CFiveAxisController::AxisGroupInitialize()
{
	return m_AxisGroup.Initialize();
}

/**
 * @brief Action under standby state, hold current positions
 *
 */
void CFiveAxisController::AxisGroupStandby()
{
	m_AxisGroup.HoldPosition();
}

/**
 * @brief Action under moving state, execute commands received from plc module
 */
void CFiveAxisController::AxisGroupMoving()
{
	if (m_CommandManager.m_Command.Meta_Data.CommandType == CommandType::eMotion)
	{
		m_AxisGroup.Move(m_CommandManager.m_Command);
	}
}

/**
 * @brief Action under handwheel state, add incremental positions calculated by plc module to current positions
 */
void CFiveAxisController::AxisGroupHandwheel()
{
	m_AxisGroup.Handwheel(m_pInputs->PanelInformation);
}

/**
 * @brief Action under limit violation state, disable motors when exceeding limits
 * 
 */
void CFiveAxisController::AxisGroupLimitViolation()
{
	//AxisGroupDisable();
	m_AxisGroup.HoldPosition();
}

void CFiveAxisController::AxisGroupFault()
{
	m_pPanelProcess->mFaultReset(); // turn off all buttons
	AxisGroupDisable();
}

void CFiveAxisController::AxisGroupRecovery()
{
	m_AxisGroup.Handwheel(m_pInputs->PanelInformation);
}

// Test Function ------------------------------------------------------------------------------------
/**
 * @brief Action under test state, execute corresponding test case triggered by host machine or plc module
 *
 */
void CFiveAxisController::AxisGroupTest()
{
	int TestMode = m_pInputs->TestInputs[0];

	switch (TestMode)
	{
	case 0:
		// Default mode
		m_AxisGroup.HoldPosition();
		m_Spindle.HoldPosition();
		
		m_SimTime = 0.0;
		for (int i = 0; i < kMaxAxisNum; i++)
		{
			m_TestInitPos[i] = m_AxisGroup.m_FdbPos[i];
		}
		break;

	case 1:
		// Only move X axis -- CSP follow Sin curve
		m_SimTime = m_SimTime + 0.001;
		double _cmdDeltaPos;
		_cmdDeltaPos = m_pInputs->TestInputs[1] * sin_(m_pInputs->TestInputs[1] * m_SimTime);
		m_AxisGroup.SingleAxisMove(AxisNum::X_Axis, m_TestInitPos[0] + _cmdDeltaPos, OpMode::CSP);
		break;

	case 2:
		// Only move Y axis -- CSP
		m_AxisGroup.SingleAxisMove(AxisNum::Y_Axis, m_pInputs->TestInputs[2], OpMode::CSP);
		break;

	case 3:
		// Only move Z axis -- CSP
		m_AxisGroup.SingleAxisMove(AxisNum::Z_Axis, m_pInputs->TestInputs[3], OpMode::CSP);
		break;

	case 4:
		// Only move X axis -- CST
		m_AxisGroup.SingleAxisMove(AxisNum::X_Axis, m_pInputs->TestInputs[4], OpMode::CST);
		break;

	case 5:
		
		break;

	case 6:
		// Eliminate deviation of gantry
		m_AxisGroup.ReturnToZeroPoint(AxisNum::X_Axis);
		break;

	case 7:
		// Only move X axis -- CSV
		m_SimTime = m_SimTime + 0.001;
		double _cmdVel;
		_cmdVel = m_pInputs->TestInputs[7] * sin_(m_pInputs->TestInputs[7] * m_SimTime * 0.3);
		m_AxisGroup.SingleAxisMove(AxisNum::X_Axis, _cmdVel, OpMode::CSV);
		break;

	case 8:
		// Only move X axis -- CST
		m_SimTime = m_SimTime + 0.001;
		double _cmdTor;
		_cmdTor = m_pInputs->TestInputs[8] * sin_(m_pInputs->TestInputs[8] * m_SimTime * 0.15);
		m_AxisGroup.SingleAxisMove(AxisNum::X_Axis, _cmdTor, OpMode::CST);
		break;

	case 9:
		break;

	default:
		
		break;
	}
}

bool CFiveAxisController::AxisGroupEnable()
{
	return m_AxisGroup.Enable();
}

bool CFiveAxisController::AxisGroupDisable()
{
	return m_AxisGroup.Disable();
}

bool CFiveAxisController::SpindleInitialize()
{
	return m_Spindle.Initialize();
}

void CFiveAxisController::SpindleStandby()
{
	m_Spindle.HoldPosition(); // or set velocity to zero
}

void CFiveAxisController::SpindleMoving()
{
	if(m_CommandManager.m_Command.Meta_Data.CommandType == CommandType::eOther)
	{
		m_Spindle.Move(m_CommandManager.m_Command);
	}
}

void CFiveAxisController::SpindleFault()
{
	m_pPanelProcess->mFaultReset(); // turn off all buttons
	SpindleDisable();
}

void CFiveAxisController::SpindleTest()
{
	int TestMode = m_pInputs->TestInputs[10];

	switch (TestMode)
	{
	case 0:
		// Default mode
		m_Spindle.HoldPosition();
		m_SimTimeSpindle = 0.0;
		break;

	case 1:
		// Spindle CSV Test
		m_Spindle.Move(m_pInputs->TestInputs[11], OpMode::CSV);
		break;

	case 2:
		if (m_SimTimeSpindle < 5.0) 
		{
			m_Spindle.Move(m_pInputs->TestInputs[12], OpMode::CSV);
		}
		else
		{
			m_Spindle.Move(-m_pInputs->TestInputs[12], OpMode::CSV);
		}

		if (m_SimTimeSpindle < 10.0)
		{
			m_SimTimeSpindle = m_SimTimeSpindle + 0.001;
		}
		else
		{
			m_SimTimeSpindle = 0.0;
		}
		break;

	case 3:
		break;

	case 4:
		break;

	case 5:
		break;

	default:
		break;
	}
}

bool CFiveAxisController::SpindleEnable()
{
	return m_Spindle.Enable();
}

bool CFiveAxisController::SpindleDisable()
{
	return m_Spindle.Disable();
}

SystemState CFiveAxisController::SpindleSafetyCheck()
{
	// TODO
	return SystemState::eIdle;
}

bool CFiveAxisController::IsServoButtonOn()
{
	return m_pInputs->PanelInformation.ServoOn || m_pInputs->mockButton[3]/* For quick test*/;
}

bool CFiveAxisController::IsHandwheelStateSelected()
{
	return (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum != 0);
}

bool CFiveAxisController::IsTestStateSelected()
{
	return m_pInputs->mockButton[9];
}

bool CFiveAxisController::IsResetSelected()
{
	return m_pInputs->PanelInformation.Reset;
}

bool CFiveAxisController::IsMovingStarted()
{
	m_bMovingFinish = false;
	return m_CommandManager.Initialize();
}

bool CFiveAxisController::IsMovingFinished()
{
	// Currently, only a stop motion instruction is placed at the end of the ringbuffer, 
	// which applies to AxisGroup and Spindle.
	return (m_CommandManager.m_Command.Meta_Data.CommandType == CommandType::eSignalFinalized);
}

void CFiveAxisController::NotifyPlcResetSoE(bool _bSignal)
{
	// Invoke plc method
	m_pSoEProcess->mReset(_bSignal);
}

bool CFiveAxisController::IsSpindleTestSelected()
{
	return m_pInputs->mockButton[8];
}

void CFiveAxisController::GetCommand()
{
	if (m_AxisGroupState == SystemState::eMoving && m_SpindleState == SystemState::eMoving)
	{
		m_CommandManager.GetCommand();
	}
	else
	{
		// TODO: report error
	}
}

void CFiveAxisController::ResetCommand()
{
	m_CommandManager.Reset();
	m_bMovingStart = false;
}

void CFiveAxisController::HandwheelCommandReset()
{
	m_pPanelProcess->mHandwheelReset();
}

int CFiveAxisController::GetBufferSize()
{
	return m_CommandManager.GetBufferSize();
}

bool CFiveAxisController::SetBufferStartLength(int _min)
{
	m_CommandManager.SetMinLengthToStart(_min);
	return (m_CommandManager.m_MinStartLength > 0);
}

bool CFiveAxisController::IsReadyToMove()
{
	return (m_AxisGroupState == SystemState::eStandby && m_SpindleState == SystemState::eStandby);
}

bool CFiveAxisController::IsReadyToReceiveCmd()
{
	return (m_pInputs->CommandWriteIndex == 0);
}

void CFiveAxisController::SetRecoveryFlag(bool _bEnter)
{
	m_bEnterRecoveryState = _bEnter;
}

void CFiveAxisController::PanelProcess()
{
	m_pPanelProcess->mInput(); // Parse input signal
	m_pPanelProcess->mOutput(); // Update button status
}

void CFiveAxisController::MatrixTest()
{
	Matrix<200, 200> _matrix;
	Vector<200> _vec;

	Vector<200> _nRet;

	_nRet = _matrix * _vec;
}
