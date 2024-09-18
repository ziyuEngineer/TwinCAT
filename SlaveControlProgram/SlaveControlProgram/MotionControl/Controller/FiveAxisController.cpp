#include "../TcPch.h"
#pragma hdrstop
#include "FiveAxisController.h"

CFiveAxisController::CFiveAxisController()
{
	m_CurrSysState = SystemState::eIdle;

	permittedTransitionsIn[SystemState::eIdle] = { eInitialization };
	permittedTransitionsIn[SystemState::eInitialization] = { eDisabled };
	permittedTransitionsIn[SystemState::eDisabled] = { eStandby, eEmergency };
	permittedTransitionsIn[SystemState::eStandby] = { eDisabled, eHandwheel, eMoving, eTest, eLimitViolation, eFault };
	permittedTransitionsIn[SystemState::eMoving] = { eStandby, eDisabled, eLimitViolation, eFault };
	permittedTransitionsIn[SystemState::eHandwheel] = { eStandby, eDisabled, eLimitViolation, eFault };
	permittedTransitionsIn[SystemState::eLimitViolation] = { eStandby, eFault };
	permittedTransitionsIn[SystemState::eFault] = { eStandby };
	permittedTransitionsIn[SystemState::eEmergency] = { eDisabled };
	permittedTransitionsIn[SystemState::eTest] = { eStandby, eLimitViolation, eFault };

}


CFiveAxisController::~CFiveAxisController()
{

}

/**
 * @brief Cyclic process inputs and outputs
 *
 */
void CFiveAxisController::Run()
{
	Input();

	SafetyCheck();

	StateControl();

	Output();
}

void CFiveAxisController::MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters)
{
	m_pInputs = _pInputs;
	m_pOutputs = _pOutputs;
	m_AxisGroup.MapParameters(_pInputs, _pOutputs, _pParameters);
	m_Spindle.MapParameters(_pInputs, _pOutputs, _pParameters);
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
	m_AxisGroup.Input();
	m_Spindle.Input();
}

/**
 * @brief Send commands or data to output channels of hardware and PLC module
 *
 */
void CFiveAxisController::Output()
{
	m_pOutputs->StateMachine.CurrentState = m_CurrSysState;

	m_AxisGroup.Output();
	m_Spindle.Output();

	for (int i = 0; i < kActualAxisNum; i++)
	{
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentPos = m_AxisGroup.m_FdbPos[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentVel = m_AxisGroup.m_FdbVel[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentTor = m_AxisGroup.m_FdbTor[i];
		m_pOutputs->AxisInfo.AxisStatus[i].CurrentMode = m_AxisGroup.m_CurrentOpMode[i];

		m_pOutputs->AxisInfo.AxisStatus[i].CommandPos = m_AxisGroup.m_Axes[i][0].m_CmdPos;
		m_pOutputs->AxisInfo.AxisStatus[i].CommandVel = m_AxisGroup.m_Axes[i][0].m_CmdPosBeforeInterpolated; // Only for observing
		m_pOutputs->AxisInfo.AxisStatus[i].CommandTor = m_AxisGroup.m_Axes[i][0].m_CmdTor;
	}

	m_pOutputs->AxisInfo.GantryDeviation = m_AxisGroup.m_GantryDeviation;
	
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentPos = m_Spindle.m_FdbPos;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentVel = m_Spindle.m_FdbVel;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentTor = m_Spindle.m_FdbTor;
	m_pOutputs->AxisInfo.AxisStatus[5].CurrentMode = m_Spindle.m_CurrentOpMode;

	m_pOutputs->AxisInfo.AxisStatus[5].CommandPos = m_Spindle.m_Axis.m_CmdVelBeforeInterpolated; // Only for observing
	m_pOutputs->AxisInfo.AxisStatus[5].CommandVel = m_Spindle.m_Axis.m_CmdVel;
	m_pOutputs->AxisInfo.AxisStatus[5].CommandTor = m_Spindle.m_Axis.m_CmdTor;
}

/**
 * @brief check if system state is abnormal.
 *
 */
void CFiveAxisController::SafetyCheck()
{
	SystemState requestState;
	if (m_CurrSysState > SystemState::eInitialization)
	{
		requestState = m_AxisGroup.SafetyCheck();
		if (requestState != SystemState::eIdle)
		{
			RequestStateChange(&m_CurrSysState, requestState);
		}
	}
}

/**
 * @brief State machine control, enables the system to perform the corresponding actions of the state machine
 */
void CFiveAxisController::StateControl()
{
	switch (m_CurrSysState)
	{
	case eIdle:
		RequestStateChange(&m_CurrSysState, SystemState::eInitialization);
		break;

	case eInitialization:
		if (!m_bInit)
		{
			StateInitialize();
		}
		else
		{
			RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
		}
		break;

	case eDisabled:
		if (m_pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (SystemEnable())
			{
				RequestStateChange(&m_CurrSysState, SystemState::eStandby);
			}
		}
		break;

	case eStandby:
		if (!m_pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (SystemDisable())
			{
				RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
			}
		}
		else if (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum != 0)// check whether axis button is pressed 
		{
			RequestStateChange(&m_CurrSysState, SystemState::eHandwheel);
		}
		else if (m_pInputs->mockButton[4]) // moving, button status from host application
		{
			RequestStateChange(&m_CurrSysState, SystemState::eMoving);
		}
		else if (m_pInputs->mockButton[9]) // test, button status from host application
		{
			RequestStateChange(&m_CurrSysState, SystemState::eTest);
		}
		else
		{
			StateStandby();
		}
		break;

	case eMoving:
		if (!m_pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (SystemDisable()){
				RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
			}
		}
		else if (m_pInputs->mockButton[4])
		{
			if (m_pInputs->CommandData.commandIndex > 0) // Local command has been updated
			{
				StateMoving();
			}
		}
		else
		{
			RequestStateChange(&m_CurrSysState, SystemState::eStandby);
		}
		break;

	case eHandwheel:
		if (!m_pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (m_AxisGroup.Disable()) {
				RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
			}
		}
		else if (m_pInputs->PanelInformation.Handwheel_EnabledAxisNum != 0)
		{
			StateHandwheel();
		}
		else
		{
			RequestStateChange(&m_CurrSysState, SystemState::eStandby);
		}
		break;

	case eLimitViolation:
		if (!m_pInputs->PanelInformation.Reset) // reset button from panel
		{
			StateLimitViolation();
		}
		else
		{
			if (m_AxisGroup.Enable()) {
				RequestStateChange(&m_CurrSysState, SystemState::eStandby);
			}
		}
		break;

	case eFault:
		if (!m_pInputs->PanelInformation.Reset) // reset button  from panel
		{
			m_pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = false;
		}
		else
		{
			// Notify plc module to Reset SoE
			m_pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = true;

			if (m_AxisGroup.Enable()) {
				RequestStateChange(&m_CurrSysState, SystemState::eStandby);
				m_pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = false;
			}
		}
		break;

	case eEmergency:
		if (m_pInputs->PanelInformation.Reset)
		{
			if (m_AxisGroup.Disable())
			{
				RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
			}
		}
		break;

	case eTest:
		if (!m_pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (SystemDisable()) {
				RequestStateChange(&m_CurrSysState, SystemState::eDisabled);
			}
		}
		else if (m_pInputs->mockButton[9])
		{
			StateTest();
		}
		else
		{
			RequestStateChange(&m_CurrSysState, SystemState::eStandby);
		}
		break;

	default:
		break;
	}
}

void CFiveAxisController::RequestStateChange(SystemState* _current_state, SystemState _request_state)
{
	auto iterator = permittedTransitionsIn.find(*_current_state);

	if (iterator->second.find(_request_state) != iterator->second.end())
	{
		*_current_state = _request_state;
	}
}

/**
 * @brief Action under initialize state, set parameters of motors
 *
 * @return true
 * @return false
 */
bool CFiveAxisController::StateInitialize()
{
	if (m_AxisGroup.Initialize() && m_Spindle.Initialize())
	{
		m_bInit = true;
	}

	return m_bInit;
}

/**
 * @brief Action under standby state, hold current positions
 *
 */
void CFiveAxisController::StateStandby()
{
	m_AxisGroup.HoldPosition();
	m_Spindle.HoldPosition();
}

/**
 * @brief Action under moving state, execute commands received from plc module
 */
void CFiveAxisController::StateMoving()
{
	m_AxisGroup.Move(m_pInputs->CommandData);
	// TODO: Distinguish command type, only axis group moves; only spindle move; move together.
}

/**
 * @brief Action under handwheel state, add incremental positions calculated by plc module to current positions
 */
void CFiveAxisController::StateHandwheel()
{
	m_AxisGroup.Handwheel(m_pInputs->PanelInformation);
}

/**
 * @brief Action under limit violation state, disable motors when exceeding limits
 * 
 */
void CFiveAxisController::StateLimitViolation()
{
	m_AxisGroup.Disable();
	m_Spindle.Disable();
}

void CFiveAxisController::StateFault()
{
	m_AxisGroup.Disable();
	m_Spindle.Disable();
}

// Test Function ------------------------------------------------------------------------------------
/**
 * @brief Action under test state, execute corresponding test case triggered by host machine or plc module
 *
 * @param m_pInputs
 * @param m_pOutputs
 */
void CFiveAxisController::StateTest()
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
		// Spindle CSV Test
		m_Spindle.Move(m_pInputs->TestInputs[5], OpMode::CSV);
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

	case 10:
		
		break;

	default:
		
		break;
	}
}

bool CFiveAxisController::SystemEnable()
{
	return m_AxisGroup.Enable() && m_Spindle.Enable();
}

bool CFiveAxisController::SystemDisable()
{
	return m_AxisGroup.Disable() && m_Spindle.Disable();
}
