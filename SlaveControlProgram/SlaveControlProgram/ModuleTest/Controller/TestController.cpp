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
	ReactionToGetParamValueButton();
	ReactionToSetParamValueButton();
	ReactionToPositioning();
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
				//m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
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
			if (!m_IsPreContinuousPassed)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupPreContinuouslyMoving;
				m_IsPreContinuousPassed = IsAxisGroupOpModeChanged();
			}
			else if (m_IsPreContinuousPassed && !m_IsContinuousSelected)
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupContinuouslyMoving;
				m_IsContinuousSelected = true;
			}
			else
			{
				m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
				UpdateContinuousMovingCommand();
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
				//m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
			}
		}
	}
}

void CTestController::ReactionToGetParamValueButton()
{
	if (m_pInputs->GetParamKpButton)
	{
		if (!m_IsGetKpButtonSelected)
		{
			if (TestGetSingleAxisKp()) { m_IsGetKpButtonSelected = true;}
		}
	}
	else { m_IsGetKpButtonSelected = false;}

	if (m_pInputs->GetParamKvButton)
	{
		if (!m_IsGetKvButtonSelected)
		{
			if (TestGetSingleAxisKv()) { m_IsGetKvButtonSelected = true; }
		}
	}
	else { m_IsGetKvButtonSelected = false; }

	if (m_pInputs->GetParamTnButton)
	{
		if (!m_IsGetTnButtonSelected)
		{
			if (TestGetSingleAxisTn()) { m_IsGetTnButtonSelected = true; }
		}
	}
	else { m_IsGetTnButtonSelected = false; }
}

void CTestController::ReactionToSetParamValueButton()
{
	if (m_pInputs->SetParamKpButton)
	{
		if (!m_IsSetKpButtonSelected)
		{
			if (TestSetSingleAxisKp())
			{
				m_IsSetKpButtonSelected = true;
			}
		}
	}
	else { m_IsSetKpButtonSelected = false; }

	if (m_pInputs->SetParamKvButton)
	{
		if (!m_IsSetKvButtonSelected)
		{
			if (TestSetSingleAxisKv())
			{
				m_IsSetKvButtonSelected = true;
			}
		}
	}
	else { m_IsSetKvButtonSelected = false; }

	if (m_pInputs->SetParamTnButton)
	{
		if (!m_IsSetTnButtonSelected)
		{
			if (TestSetSingleAxisTn())
			{
				m_IsSetTnButtonSelected = true;
			}
		}
	}
	else { m_IsSetTnButtonSelected = false; }
}

void CTestController::ReactionToPositioning()
{
	if (m_pInputs->PositionY || m_pInputs->PositionX || m_pInputs->PositionZ)
	{
		if (!m_IsPositioning)
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupPositioning;
			m_IsPositioning = true;
		}
		else
		{
			m_AxisGroupTestCase = AxisGroupCase::AxisGroupNull;
		}
	}
	else { m_IsPositioning = false; }
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

	case AxisGroupCase::AxisGroupPreContinuouslyMoving:
		TestAxisGroupChangeOpMode(OpMode::CSP); // temp use
		break;

	case AxisGroupCase::AxisGroupContinuouslyMoving:
		TestAxisGroupMove();
		break;

	case AxisGroupCase::AxisGroupStopContinuouslyMoving:
		TestAxisGroupStop();
		break;

	case AxisGroupCase::AxisGroupPositioning:
		TestAxisGroupPositioning();
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

void CTestController::TestAxisGroupPositioning()
{
	bool enabled_axis[] = {true, true, true, true, true};
	
	double pos[] = {120.0, 300.0, -300.0, 220.0, 150.0};

	m_pAxisGroupInterface->RequestMovingToTargetPos(enabled_axis, pos);
}

bool CTestController::TestGetSingleAxisKp()
{
	bool ret = false;
	
	double Kp = 0.123;
	// get z-axis param
	ret = m_pAxisGroupInterface->GetAxisVelLoopKp(2, Kp) == S_OK ? true : false;
	
	m_pOutputs->TestOutputs[0] = Kp;
	
	return ret;
}

bool CTestController::TestGetSingleAxisKv()
{
	bool ret = false;
	double Kv = 0.123;
	
	// get z-axis param
	ret = m_pAxisGroupInterface->GetAxisPosLoopKv(2, Kv) == S_OK ? true : false;

	m_pOutputs->TestOutputs[2] = Kv;

	return ret;
}

bool CTestController::TestGetSingleAxisTn()
{
	bool ret = false;
	double Tn = 0.123;

	// get z-axis param
	ret = m_pAxisGroupInterface->GetAxisVelLoopTn(2, Tn) == S_OK ? true : false;

	m_pOutputs->TestOutputs[1] = Tn;

	return ret;
}

bool CTestController::TestSetSingleAxisKp()
{
	bool ret = false;
	double Kp = 0.025;
	// set z-axis param
	ret = m_pAxisGroupInterface->SetAxisVelLoopKp(3, Kp) == S_OK ? true : false;
	return ret;
}

bool CTestController::TestSetSingleAxisTn()
{
	bool ret = false;
	double Tn = 12.5;
	// set z-axis param
	ret = m_pAxisGroupInterface->SetAxisVelLoopTn(3, Tn) == S_OK ? true : false;
	return ret;
}

bool CTestController::TestSetSingleAxisKv()
{
	bool ret = false;
	double Kv = 5.5;
	// set z-axis param
	ret = m_pAxisGroupInterface->SetAxisPosLoopKv(3, Kv) == S_OK ? true : false;
	return ret;
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
	m_pOutputs->MockManualMovingCommand.MockCommandPos = m_pInputs->MockPanel.Handwheel_Pos;
}

void CTestController::UpdateContinuousMovingCommand()
{
	// temp use
	if (m_pOutputs->MockContinuousMovingCommand.metaData.Index < 1000)
	{
		m_pOutputs->MockContinuousMovingCommand.metaData.CommandType = CommandType::eSingleAxisZ;
		m_pOutputs->MockContinuousMovingCommand.metaData.Index += 1;
		m_pOutputs->MockContinuousMovingCommand.motionCommand.Pos[2] = m_pInputs->TestAxisGroupInfo.SingleAxisInformation[3].CurrentPos + 0.01;
	}
	else
	{

	}
}

void CTestController::TestSafetyEnterRecovery()
{
	m_pSafetyInterface->EnterRecoveryState();
}

void CTestController::TestSafetyExitRecovery()
{
	m_pSafetyInterface->LeaveRecoveryState();
}