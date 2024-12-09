#include "../TcPch.h"
#pragma hdrstop
#include "TestController.h"

ISpindleInterface* CTestController::m_pSpindleInterface = nullptr;
IAxisGroupInterface* CTestController::m_pAxisGroupInterface = nullptr;
ISafetyInterface* CTestController::m_pSafetyInterface = nullptr;

CTestController::CTestController()
{
	m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
	m_SpindleTestCase = SpindleCase::SpindleNull;
	m_SafetyTestCase = SafetyModuleCase::SafetyNull;
}

CTestController::~CTestController()
{
}

void CTestController::MapParameters(ModuleTestInputs* inputs, ModuleTestOutputs* outputs)
{
	m_pInputs = inputs;
	m_pOutputs = outputs;
}

void CTestController::Run()
{
	ReactionToMockPanel();
	AxisGroupTest();
	SpindleTest();
	SafetyTest();
}

void CTestController::ReactionToMockPanel()
{
	ReactionToServoButton();
	ReactionToSpindleButton();
	ReactionToHandwheelButton();
	ReactionToRecoveryButton();
	ReactionToContinuousButton();
}

void CTestController::ReactionToServoButton()
{
	if (m_pInputs->MockPanel.ServoOn)
	{
		if (!m_IsOperationAllowed)
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupServoOn;
			m_SpindleTestCase = SpindleCase::SpindleEnable;
			m_IsOperationAllowed = IsMovingOperationAllowed();
		}
		else
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			m_SpindleTestCase = SpindleCase::SpindleNull;
		}
	}
	else if (!m_pInputs->MockPanel.ServoOn)
	{
		if (m_IsOperationAllowed)
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupServoOff;
			m_IsOperationAllowed = false;
		}
		else
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			m_SpindleTestCase = SpindleCase::SpindleNull;
		}
	}
}

void CTestController::ReactionToSpindleButton()
{
	if (m_IsOperationAllowed)
	{
		// Spindle case
		switch (m_pInputs->MockPanel.Spindle_Cmd)
		{
		case -1: // CCW Rotate
			if (!m_IsSpindleMoving)
			{
				m_SpindleTestCase = SpindleCase::SpindleRotating;
				m_IsSpindleMoving = IsSpindleMoving();
			}
			else
			{
				m_SpindleTestCase = SpindleCase::SpindleNull;
			}
			break;
		case 0: // Stop
			if (m_IsSpindleMoving)
			{
				m_SpindleTestCase = SpindleCase::SpindleStopMoving;
				m_IsSpindleMoving = IsSpindleMoving();
			}
			else
			{
				m_SpindleTestCase = SpindleCase::SpindleNull;
			}
			break;
		case 1: // CW Rotate
			if (!m_IsSpindleMoving)
			{
				m_SpindleTestCase = SpindleCase::SpindleRotating;
				m_IsSpindleMoving = IsSpindleMoving();
			}
			else
			{
				m_SpindleTestCase = SpindleCase::SpindleNull;
			}
			break;
		}
	}
}

void CTestController::ReactionToHandwheelButton()
{
	if (m_IsOperationAllowed)
	{
		if (m_pInputs->MockPanel.Handwheel_EnabledAxisNum >= AxisOrder::Axis_X)
		{
			if (!m_IsAxisGroupManualMoving)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupEnterHandwheel;
				m_IsAxisGroupManualMoving = IsAxisGroupManualMoving();
			}
			else
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
				UpdateManualMovingCommand();
			}
		}
		else
		{
			if (m_IsAxisGroupManualMoving)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupQuitHandwheel;
				m_IsAxisGroupManualMoving = IsAxisGroupManualMoving();
			}
			else
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			}
		}
	}
}

void CTestController::ReactionToRecoveryButton()
{
	if (m_pInputs->RecoveryButton)
	{
		if (!m_IsRecoverSelected)
		{
			m_SafetyTestCase = SafetyModuleCase::SafetyEnterRecovery;
			m_IsRecoverSelected = true;
		}
		else
		{
			m_SafetyTestCase = SafetyModuleCase::SafetyNull;
		}
	}
	else
	{
		if (m_IsRecoverSelected)
		{
			m_SafetyTestCase = SafetyModuleCase::SafetyExitRecovery;
			m_IsRecoverSelected = false;
		}
		else
		{
			m_SafetyTestCase = SafetyModuleCase::SafetyNull;
		}
	}
}

void CTestController::ReactionToContinuousButton()
{
	if (m_IsOperationAllowed)
	{
		if (m_pInputs->ContinuousButton)
		{
			if (!m_IsContinuousSelected)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupContinuouslyMoving;
				m_IsContinuousSelected = true;
			}
			else
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			}
		}
		else
		{
			if (m_IsContinuousSelected)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupStopContinuouslyMoving;
				m_IsContinuousSelected = false;
			}
			else
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			}
		}
	}
}

void CTestController::AxisGroupTest()
{
	switch (m_AxisGroupTestCase)
	{
	case AxisGroupCase::AxisGroupNull:
		// Default mode
		// Do nothing
		break;

	case AxisGroupCase::AxisGroupServoOn:
		TestAxisGroupServoOn();
		break;

	case AxisGroupCase::AxisGroupServoOff:
		TestAxisGroupServoOff();
		break;

	case AxisGroupCase::AxisGroupEnterHandwheel:
		TestAxisGroupEnterHandwheel();
		break;

	case AxisGroupCase::AxisGroupQuitHandwheel:
		TestAxisGroupQuitHandwheel();
		break;

	case AxisGroupCase::AxisGroupContinuouslyMoving:
		TestAxisGroupMove();
		break;

	case AxisGroupCase::AxisGroupStopContinuouslyMoving:
		TestAxisGroupStop();
		break;

	default:
		break;
	}
}

void CTestController::SpindleTest()
{
	switch (m_SpindleTestCase)
	{
	case SpindleCase::SpindleNull:
		// Default mode
		// Do nothing
		break;

	case SpindleCase::SpindleEnable:
		TestSpindleEnable();
		break;

	case SpindleCase::SpindleRotating:
		if(m_pInputs->MockPanel.Spindle_Cmd == 1)
		{ 
			TestSpindleRotate(10.0);//rps
		}
		else
		{
			TestSpindleRotate(-10.0);//rps
		}
		break;

	case SpindleCase::SpindleLocating:
		break;

	case SpindleCase::SpindleStopMoving:
		TestSpindleStop();
		break;

	default:
		break;
	}
}

void CTestController::SafetyTest()
{
	switch (m_SafetyTestCase)
	{
	case SafetyModuleCase::SafetyNull:
		break;

	case SafetyModuleCase::SafetyEnterRecovery:
		TestSafetyEnterRecovery();
		break;

	case SafetyModuleCase::SafetyExitRecovery:
		TestSafetyExitRecovery();
		break;

	default:
		break;
	}
}

void CTestController::TestSpindleRotate(double vel)
{
	SpindleRot spindle_rot;
	spindle_rot.TargetVel = vel;
	m_pSpindleInterface->RequestRotating(spindle_rot);
}

void CTestController::TestSpindleStop()
{
	m_pSpindleInterface->RequestStop();
}

void CTestController::TestSpindleEnable()
{
	m_pSpindleInterface->RequestSpindleEnable();
}

bool CTestController::IsSpindleMoving()
{
	bool is_moving = false;
	m_pSpindleInterface->IsSpindleMoving(is_moving);
	return is_moving;
}

void CTestController::TestAxisGroupServoOn()
{
	m_pAxisGroupInterface->RequestAxisGroupEnable();
}

void CTestController::TestAxisGroupServoOff()
{
	m_pAxisGroupInterface->RequestAxisGroupDisable();
}

void CTestController::TestAxisGroupChangeOpMode(OpMode mode)
{
	m_pAxisGroupInterface->RequestChangeOpMode(mode);
}

void CTestController::TestAxisGroupMove()
{
	m_pAxisGroupInterface->RequestContinuousMoving();
}

void CTestController::TestAxisGroupStop()
{
	m_pAxisGroupInterface->RequestStop();
}

void CTestController::TestAxisGroupEnterHandwheel()
{
	m_pAxisGroupInterface->RequestEnterHandwheel();
}

void CTestController::TestAxisGroupQuitHandwheel()
{
	memset(&m_pOutputs->MockManualMovingCommand, 0, sizeof(m_pOutputs->MockManualMovingCommand));
	m_pAxisGroupInterface->RequestQuitHandwheel();
}

void CTestController::TestAxisGroupResetError()
{
	m_pAxisGroupInterface->RequestReset();
}

bool CTestController::IsAxisGroupOpModeChanged()
{
	bool is_changed = false;
	m_pAxisGroupInterface->IsAxisGroupOpModeSwitched(is_changed);
	return is_changed;
}

bool CTestController::IsAxisGroupDisabled()
{
	bool is_disabled = false;
	m_pAxisGroupInterface->IsAxisGroupDisabled(is_disabled);
	return is_disabled;
}

bool CTestController::IsAxisGroupManualMoving()
{
	bool is_moving = false;
	m_pAxisGroupInterface->IsAxisGroupManualMoving(is_moving);
	return is_moving;
}

bool CTestController::IsMovingOperationAllowed()
{
	bool is_axisgroup_standby = false;
	bool is_spindle_enable = false;
	m_pAxisGroupInterface->IsAxisGroupStandby(is_axisgroup_standby);
	m_pSpindleInterface->IsSpindleEnable(is_spindle_enable);

	return is_axisgroup_standby && is_spindle_enable;
}

void CTestController::UpdateManualMovingCommand()
{
	m_pOutputs->MockManualMovingCommand.SelectedAxis = m_pInputs->MockPanel.Handwheel_EnabledAxisNum;
	memcpy(&m_pOutputs->MockManualMovingCommand.MockCommandPos, &m_pInputs->MockPanel.Handwheel_dPos, sizeof(double[5]));
}

void CTestController::UpdateContinuousMovingCommand()
{

}

void CTestController::TestSafetyEnterRecovery()
{
	m_pSafetyInterface->EnterRecoveryState();
}

void CTestController::TestSafetyExitRecovery()
{
	m_pSafetyInterface->LeaveRecoveryState();
}