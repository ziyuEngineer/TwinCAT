#include "../TcPch.h"
#pragma hdrstop
#include "ControllerBase.h"

CControllerBase::CControllerBase()
{
	m_CurrSysState = SystemState::eIdle;

	// 定义状态转换规则
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

CControllerBase::~CControllerBase()
{

}

/**
 * @brief Cyclic process inputs and outputs
 * 
 * @param pInputs 
 * @param pOutputs 
 */
void CControllerBase::Run(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs)
{
	Input(pInputs);

	SafetyCheck();

	StateControl(pInputs, pOutputs);

	Output(pOutputs);
}

/**
 * @brief Process input signals from hardware and data from PLC module
 * 
 * @param pInputs 
 */
void CControllerBase::Input(MotionControlInputs* pInputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].m_StatusWord = pInputs->DriverInputs[i][j].StatusWord;

			AxisInst[i][j].m_FdbPosVal = pInputs->DriverInputs[i][j].ActualAbsPos;
			AxisInst[i][j].m_FdbVelVal = pInputs->DriverInputs[i][j].ActualVel;
			AxisInst[i][j].m_FdbTorVal = pInputs->DriverInputs[i][j].ActualTor;
				   
			AxisInst[i][j].m_ErrorCode = pInputs->DriverInputs[i][j].ErrorCode;
			AxisInst[i][j].m_WarningCode = pInputs->DriverInputs[i][j].WarningCode;

			AxisInst[i][j].m_DigitalInput = pInputs->DriverInputs[i][j].DigitalInput_1; // Status of optoelectronic switch

			//m_FdbPos[i][j] = AxisInst[i][j].GetFeedbackPosition();
			m_FdbPos[i][j] = m_FdbPosFilter[i][j].process(AxisInst[i][j].GetFeedbackPosition());
			m_FdbTor[i][j] = AxisInst[i][j].GetFeedbackTorque();
			m_FdbVel[i][j] = AxisInst[i][j].GetFeedbackVelocity();
			m_ActualOpMode[i][j] = AxisInst[i][j].GetActualOperationMode();

			double dTempJerk;
			AxisInst[i][j].ComputeAcceleration(m_FdbVel[i][j], &m_FdbAcc[i][j], &dTempJerk);
		}
	}
}

/**
 * @brief Send commands or data to output channels of hardware and PLC module
 * 
 * @param pOutputs 
 */
void CControllerBase::Output(MotionControlOutputs* pOutputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].m_ControlWord;
			switch (AxisInst[i][j].m_OperationMode)
			{
			case CSP:
			{
				pOutputs->DriverOutputs[i][j].TargetPosition = AxisInst[i][j].SendTargetPosition(m_CmdPos[i][j]);
				break;
			}
			case CST:
			{
				pOutputs->DriverOutputs[i][j].TargetTorque = AxisInst[i][j].SendTargetTorque(m_CmdTor[i][j]);
				break;
			}
			case CSV:
			{
				pOutputs->DriverOutputs[i][j].TargetVelocity = AxisInst[i][j].SendTargetVelocity(m_CmdVel[i][j]);
				break;
			}
			default:
				// Control word should have a non-zero value
				AxisInst[i][j].ControlUnitSync();
				pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].m_ControlWord;
				break;
			}
		}
	}

	pOutputs->StateMachine.CurrentState = m_CurrSysState;

	// For observing variables, will be deprecated later.
	// Z-axis
	pOutputs->AxisInfoOutputs[0] = m_FdbPos[0][0];
	pOutputs->AxisInfoOutputs[1] = m_FdbVel[0][0];
	pOutputs->AxisInfoOutputs[2] = m_FdbTor[0][0];
	pOutputs->AxisInfoOutputs[3] = m_ActualOpMode[0][0];

	// Y-axis
	pOutputs->AxisInfoOutputs[4] = m_FdbPos[1][0];
	pOutputs->AxisInfoOutputs[5] = m_FdbVel[1][0];
	pOutputs->AxisInfoOutputs[6] = m_FdbTor[1][0];
	pOutputs->AxisInfoOutputs[7] = m_ActualOpMode[1][0];

	/*pOutputs->AxisInfoOutputs[8] = m_FdbPos[2][0];
	pOutputs->AxisInfoOutputs[9] = m_FdbVel[2][0];
	pOutputs->AxisInfoOutputs[10] = m_FdbTor[2][0];
	pOutputs->AxisInfoOutputs[11] = m_ActualOpMode[2][0];

	pOutputs->AxisInfoOutputs[12] = m_FdbPos[2][1];
	pOutputs->AxisInfoOutputs[13] = m_FdbVel[2][1];
	pOutputs->AxisInfoOutputs[14] = m_FdbTor[2][1];
	pOutputs->AxisInfoOutputs[15] = m_ActualOpMode[2][1];*/

	// Generate error code
	// TODO
}

/**
 * @brief check if system state is abnormal.
 * 
 */
void CControllerBase::SafetyCheck()
{
	if (m_CurrSysState > eInitialization)
	{
		// Check status of axis
		for (int i = 0; i < kAxisNum; i++)
		{
			for (int j = 0; j < kJointNum; j++)
			{
				if (AxisInst[i][j].IsLimitExceeded())
				{
					RequestStateChangeTo(SystemState::eLimitViolation);
				}
				else if (AxisInst[i][j].IsEmergencyState())
				{
					RequestStateChangeTo(SystemState::eEmergency);
				}
				else if (AxisInst[i][j].IsFaultState())
				{
					RequestStateChangeTo(SystemState::eFault);
				}
			}
		}
	}
}

/**
 * @brief State transition logic determination based on pre-defined transitionRules
 *
 * @param _request_state
 */
void CControllerBase::RequestStateChangeTo(SystemState _request_state)
{
	auto iterator = permittedTransitionsIn.find(m_CurrSysState);

	if (iterator->second.find(_request_state) != iterator->second.end())
	{
		m_CurrSysState = _request_state;
	}
}

/**
 * @brief State machine control, enables the system to perform the corresponding actions of the state machine
 * 
 * @param pInputs 
 * @param pOutputs 
 */
void CControllerBase::StateControl(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs)
{
	switch (m_CurrSysState)
	{
	case eIdle:
		RequestStateChangeTo(SystemState::eInitialization);
		break;

	case eInitialization:
		if (!m_bInit)
		{
			if (GetMotionCtrlParam(pInputs))
			{
				StateInitialize();
			}
		}
		else
		{
			RequestStateChangeTo(SystemState::eDisabled);
		}
		break;

	case eDisabled:
		//if (pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		if (pInputs->mockButton[3]) 
		{
			if (MotorEnable())
			{
				for (int i = 0; i < kAxisNum; i++)
				{
					for (int j = 0; j < kJointNum; j++)
					{
						// update initial positions
						m_InitPos[i][j] = AxisInst[i][j].GetFeedbackPosition();
					}
				}
				RequestStateChangeTo(SystemState::eStandby);
			}
		}
		break;

	case eStandby:
		if (pInputs->PanelInformation.AxisEnable[0] || pInputs->PanelInformation.AxisEnable[1] || pInputs->PanelInformation.AxisEnable[2])// check whether axis button is pressed 
		{
			// Reset m_HandwheelInitPos upon entering eHandwheel state
			memcpy(&m_HandwheelInitPos, &m_FdbPos, sizeof(m_FdbPos));
			RequestStateChangeTo(SystemState::eHandwheel);
		}
		else if (pInputs->mockButton[4]) // moving, button status from host application
		{
			// Reset m_MovingInitPos upon entering eMoving state
			memcpy(&m_MovingInitPos, &m_FdbPos, sizeof(m_FdbPos));
			RequestStateChangeTo(SystemState::eMoving);
		}
		else if (pInputs->mockButton[9]) // test, button status from host application
		{
			RequestStateChangeTo(SystemState::eTest);
		}
		//else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		else if (!pInputs->mockButton[3])
		{
			if (MotorDisable())
			{
				RequestStateChangeTo(SystemState::eDisabled);
			}
		}
		else
		{
			StateStandby();
		}
		break;

	case eMoving:
		if (pInputs->mockButton[4])
		{
			if (pInputs->CommandData.commandIndex > 0) // Local command has been updated
			{
				StateMoving(pInputs);
			}
		}
		//else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		//{
		//	if (MotorDisable()){
		//		RequestStateChangeTo(SystemState::eDisabled);
		//	}
		//}
		else
		{
			RequestStateChangeTo(SystemState::eStandby);
		}
		break;

	case eHandwheel:
		if (pInputs->PanelInformation.AxisEnable[0] || pInputs->PanelInformation.AxisEnable[1] || pInputs->PanelInformation.AxisEnable[2])
		{
			StateHandwheel(pInputs);
		}
		//else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		//{
		//	if (MotorDisable()) {
		//		RequestStateChangeTo(SystemState::eDisabled);
		//	}
		//}
		else
		{
			RequestStateChangeTo(SystemState::eStandby);
		}
		break;

	case eLimitViolation:
		if (!pInputs->mockButton[6]) // reset button
		{
			StateLimitViolation();
		}
		else
		{
			if (MotorEnable()) {
				RequestStateChangeTo(SystemState::eStandby);
			}
		}
		break;

	case eFault:
		if (!pInputs->mockButton[6]) // reset button
		{
			pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = false;
		}
		else
		{
			// Notify plc module to Reset SoE
			pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = true;

			if (MotorEnable()) {
				RequestStateChangeTo(SystemState::eStandby);
				pOutputs->StateMachine.StateFlag[static_cast<int>(eFault)] = false;
			}
		}
		break;

	case eEmergency:
		if (pInputs->mockButton[6])
		{
			if (MotorDisable())
			{
				RequestStateChangeTo(SystemState::eDisabled);
			}
		}
		break;

	case eTest:
		if (pInputs->mockButton[9])
		{
			StateTest(pInputs, pOutputs);
		}
		//else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		//{
		//	if (MotorDisable()) {
		//		RequestStateChangeTo(SystemState::eDisabled);
		//	}
		//}
		else
		{
			RequestStateChangeTo(SystemState::eStandby);
		}
		break;

	default:
		break;
	}
}

/**
 * @brief Action under initialize state, set parameters of motors
 * 
 * @return true 
 * @return false 
 */
bool CControllerBase::StateInitialize()
{
	if (SetMotorParam())
	{
		m_bInit = true;
	}

	return m_bInit;
}

/**
 * @brief Get motion control parameters loaded by PLC module
 * 
 * @param pInputs 
 * @return true 
 * @return false 
 */
bool CControllerBase::GetMotionCtrlParam(MotionControlInputs* pInputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_AxisParam[i][j].abs_dir = pInputs->MotionCtrlParam[i][j].AbsEncDir;
			m_AxisParam[i][j].abs_encoder_res = pInputs->MotionCtrlParam[i][j].AbsEncRes;
			m_AxisParam[i][j].abs_zero_pos = pInputs->MotionCtrlParam[i][j].AbsZeroPos;
										 
			m_AxisParam[i][j].rated_curr = pInputs->MotionCtrlParam[i][j].RatedCurrent;
			m_AxisParam[i][j].rated_tor	= pInputs->MotionCtrlParam[i][j].RatedTorque;
			m_AxisParam[i][j].tor_dir = pInputs->MotionCtrlParam[i][j].TorDir;
			
			// Add this to avoid 'SSE invalid Operation' since variable rated_curr may be zero at first and denominator should not be zero
			if (m_AxisParam[i][j].rated_curr > kEPSILON)
			{
				m_AxisParam[i][j].tor_cons = m_AxisParam[i][j].rated_tor / m_AxisParam[i][j].rated_curr;
			}
											 
			m_AxisParam[i][j].abs_pos_ub = pInputs->MotionCtrlParam[i][j].PosUpperLimit;
			m_AxisParam[i][j].abs_pos_lb = pInputs->MotionCtrlParam[i][j].PosLowerLimit;
			m_AxisParam[i][j].transmission_ratio = pInputs->MotionCtrlParam[i][j].TransmissionRatio;

			m_AxisParam[i][j].tor_pdo_max = pInputs->MotionCtrlParam[i][j].TorPdoMax;
			m_AxisParam[i][j].additive_tor = pInputs->MotionCtrlParam[i][j].AdditiveTorque;

			m_AxisParam[i][j].abs_encoder_type = pInputs->MotionCtrlParam[i][j].AbsEncType;
			
			m_AxisParam[i][j].positive_hard_bit = pInputs->MotionCtrlParam[i][j].PositiveHardBit;
			m_AxisParam[i][j].negative_hard_bit = pInputs->MotionCtrlParam[i][j].NegativeHardBit;
		}
	}

	if (m_AxisParam[kAxisNum - 1][kJointNum - 1].tor_pdo_max > kEPSILON)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief Set motor parameters of each motor
 * 
 * @return true 
 * @return false 
 */
bool CControllerBase::SetMotorParam()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			if(!AxisInst[i][j].SetAxisParam(m_AxisParam[i][j]))
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Servo on all motors
 * 
 * @return true 
 * @return false 
 */
bool CControllerBase::MotorEnable()
{
	MotorHoldPosition();

	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			if (!AxisInst[i][j].Enable())
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Servo off all motors
 * 
 * @return true 
 * @return false 
 */
bool CControllerBase::MotorDisable()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			if (!AxisInst[i][j].Disable())
			{
				return false;
			}
		}
	}
	return true;
}

void CControllerBase::MotorHoldPosition()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].SetOperationMode(CSP);
			m_CmdPos[i][j] = m_FdbPos[i][j];
		}
	}
}

/**
 * @brief Action under standby state, hold current positions
 * 
 */
void CControllerBase::StateStandby()
{
	MotorHoldPosition();
}

/**
 * @brief Action under moving state, execute commands received from plc module
 * 
 * @param pInputs 
 */
void CControllerBase::StateMoving(MotionControlInputs* pInputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].SetOperationMode(static_cast<OpMode>(pInputs->CommandData.opMode[i]));

			m_CmdPos[i][j] = m_MovingInitPos[i][j] + pInputs->CommandData.pos[i];

			m_CmdVel[i][j] = pInputs->CommandData.vel[i];

			// Only for testing CST, will use K matrix instead in the future
			m_CmdTor[i][j] = pInputs->CommandData.acc[i];
			/*m_CmdTor[i][j] = KmatrixController(m_FdbPos[i][j], m_FdbVel[i][j], m_FdbAcc[i][j],
				pInputs->CommandData.pos[i], pInputs->CommandData.vel[i], pInputs->CommandData.acc[i],
				pInputs->CommandData.kp[i], pInputs->CommandData.kv[i], pInputs->CommandData.ka[i]);*/
		}
	}
}

/**
 * @brief Action under handwheel state, add incremental positions calculated by plc module to current positions 
 * 
 * @param pInputs 
 */
void CControllerBase::StateHandwheel(MotionControlInputs* pInputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].SetOperationMode(CSP);
			m_CmdPos[i][j] = m_HandwheelInitPos[i][j] + pInputs->PanelInformation.Handwheel_dPos[i];
		}
	}
}

/**
 * @brief Action under limit violation state, disable motors when exceeding limits
 *
 */
void CControllerBase::StateLimitViolation()
{
	MotorDisable();
}

// Test Function ------------------------------------------------------------------------------------
/**
 * @brief Action under test state, execute corresponding test case triggered by host machine or plc module
 * 
 * @param pInputs 
 * @param pOutputs 
 */
void CControllerBase::StateTest(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs)
{
	int TestMode = pInputs->TestInputs[0];

	switch (TestMode)
	{
	case 0:
		TestDefault();
		memcpy(&m_TestFdbPos, &m_FdbPos, sizeof(m_FdbPos));
		m_SimTime = 0.0;
		break;

	case 1:
		TestCSP(pInputs->TestInputs[1], 1); // Only move Z axis
		break;

	case 2:
		TestCSP(pInputs->TestInputs[2], 2); // Only move Y axis
		break;

	case 3:
		TestCSP(pInputs->TestInputs[3], 3); // Only move X axis
		break;

	case 4:
		TestCST(pInputs->TestInputs[2]);
		break;

	case 5:
		TestCSV(pInputs->TestInputs[3]);
		break;

	case 6:
		TestMoveSin(pInputs);
		break;

	case 7:
		TestMoveSinCSP(pInputs->TestInputs[5], pInputs->TestInputs[5]);
		m_SimTime += 0.001;
		break;
	
	case 8:
		TestSinCST(pInputs->TestInputs[6], pInputs->TestInputs[6] / 10.0);
		m_SimTime += 0.001;
		break;

	case 9:
		TestSinCSV(pInputs->TestInputs[7], pInputs->TestInputs[7]);
		m_SimTime += 0.001;
		break;

	case 10:
		TestBrake();
		break;

	default:
		TestDefault();
		break;
	}
}

/**
 * @brief Test move according to sin curve
 * 
 * @param pInputs 
 */
void CControllerBase::TestMoveSin(MotionControlInputs* pInputs)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].SetOperationMode(CST);
			m_CmdTor[i][j] = KmatrixController(m_FdbPos[i][j], m_FdbVel[i][j], m_FdbAcc[i][j],
				pInputs->CommandData.pos[i], pInputs->CommandData.vel[i], pInputs->CommandData.acc[i],
				pInputs->CommandData.kp[i], pInputs->CommandData.kv[i], pInputs->CommandData.ka[i]);
		}
	}
}

void CControllerBase::TestMoveSinCSP(double _A, double _freq)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdPos[i][j] = m_TestFdbPos[i][j] + _A * sin_(_freq * m_SimTime);
			AxisInst[i][j].SetOperationMode(CSP);
		}
	}
}

/**
 * @brief Test CSP
 * 
 * @param length absolute positions, unit mm
 */
void CControllerBase::TestCSP(double _pos, int _axis)
{
	/*for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdPos[i][j] = _pos;
			AxisInst[i][j].SetOperationMode(CSP);
		}
	}*/
	switch (_axis - 1)
	{
	case 0: // Z axis
		m_CmdPos[0][0] = _pos;
		AxisInst[0][0].SetOperationMode(CSP);
		break;

	case 1: // Y axis
		m_CmdPos[1][0] = _pos;
		AxisInst[1][0].SetOperationMode(CSP);
		break;

	case 2:
		/*m_CmdPos[2][0] = _pos;
		AxisInst[2][0].SetOperationMode(CSP);
		m_CmdPos[2][1] = _pos;
		AxisInst[2][1].SetOperationMode(CSP);*/
		break;
	}
}

void CControllerBase::TestSinCSV(double _A, double _freq)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdVel[i][j] = _A * sin_(_freq * m_SimTime);
			AxisInst[i][j].SetOperationMode(CSV);
		}
	}
}

/**
 * @brief Test CSV
 * 
 * @param _vel command velocity, unit mm/s
 */
void CControllerBase::TestCSV(double _vel)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdVel[i][j] = _vel;
			AxisInst[i][j].SetOperationMode(CSV);
		}
	}
}

/**
 * @brief Brake by setting torque to zero
 * 
 */
void CControllerBase::TestBrake()
{
	TestCST(0.0);
}

/**
 * @brief Default test case, hold position
 * 
 */
void CControllerBase::TestDefault()
{
	MotorHoldPosition();
}

void CControllerBase::TestSinCST(double _A, double _freq)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdTor[i][j] = _A * sin_(_freq * m_SimTime);
			AxisInst[i][j].SetOperationMode(CST);
		}
	}
}

/**
 * @brief Test CST
 * 
 * @param _tor command torque, unit N.m
 */
void CControllerBase::TestCST(double _tor)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdTor[i][j] = _tor;
			AxisInst[i][j].SetOperationMode(CST);
		}
	}
}

/**
 * @brief Calculate control torque according to inputs
 * 
 * @param fdbPos 
 * @param fdbVel 
 * @param fdbAcc 
 * @param cmdPos 
 * @param cmdVel 
 * @param cmdAcc 
 * @param Kp 
 * @param Kv 
 * @param Ka 
 * @return double 
 */
double CControllerBase::KmatrixController(double fdbPos, double fdbVel, double fdbAcc, double cmdPos, double cmdVel, double cmdAcc, double Kp, double Kv, double Ka)
{
	double cmdTor = 0.0;

	double dPos = cmdPos - fdbPos;
	double dVel = cmdVel - fdbVel;
	double dAcc = cmdAcc - fdbAcc;

	cmdTor = Kp * dPos + Kv * dVel + Ka * dAcc;

	return cmdTor;
}