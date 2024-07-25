#include "../TcPch.h"
#pragma hdrstop
#include "ControllerBase.h"

CControllerBase::CControllerBase()
{
	mCurrSysState = SystemState::eIdle;
	mSysStateMachine = SystemState::eIdle;
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
			AxisInst[i][j].StatusWord = pInputs->DriverInputs[i][j].StatusWord;

			AxisInst[i][j].FdbPosVal = pInputs->DriverInputs[i][j].ActualAbsPos;
			AxisInst[i][j].FdbVelVal = pInputs->DriverInputs[i][j].ActualVel;
			AxisInst[i][j].FdbTorVal = pInputs->DriverInputs[i][j].ActualTor;
				   
			AxisInst[i][j].ErrorCode = pInputs->DriverInputs[i][j].ErrorCode;
			AxisInst[i][j].WarningCode = pInputs->DriverInputs[i][j].WarningCode;

			m_FdbPos[i][j] = AxisInst[i][j].GetFeedbackPosition();
			m_FdbTor[i][j] = AxisInst[i][j].GetFeedbackTorque();
			m_FdbVel[i][j] = AxisInst[i][j].GetFeedbackVelocity();

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
			if (AxisInst[i][j].IsEnableState())
			{
				switch (AxisInst[i][j].OperationMode)
				{
				case CSP:
				{
					AxisInst[i][j].SetOperationMode(CSP);
					pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].ControlWord;
					pOutputs->DriverOutputs[i][j].TargetPosition = AxisInst[i][j].SendTargetPosition(m_CmdPos[i][j]);
					break;
				}
				case CSV:
				{
					AxisInst[i][j].SetOperationMode(CSV);
					pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].ControlWord;
					pOutputs->DriverOutputs[i][j].TargetVelocity = AxisInst[i][j].SendTargetVelocity(m_CmdVel[i][j]);
					break;
				}
				case CST:
				{
					AxisInst[i][j].SetOperationMode(CST);
					pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].ControlWord;
					pOutputs->DriverOutputs[i][j].TargetTorque = AxisInst[i][j].SendTargetTorque(m_CmdTor[i][j]);
					break;
				}
				default:
					break;
				}
			}
			else
			{
				// Control word should have a non-zero value
				AxisInst[i][j].ServoOff();
				pOutputs->DriverOutputs[i][j].ControlWord = AxisInst[i][j].ControlWord;
			}
		}
	}

	// For observing variables, will be deprecated later.
	pOutputs->TestOutputs[6] = m_FdbVel[0][0];
	pOutputs->TestOutputs[7] = m_CmdVel[0][0];
	pOutputs->TestOutputs[8] = m_FdbPos[0][0];
	pOutputs->TestOutputs[9] = m_CmdPos[0][0];

	// Generate error code
	// TODO
}

/**
 * @brief State machine control, enables the system to perform the corresponding actions of the state machine
 * 
 * @param pInputs 
 * @param pOutputs 
 */
void CControllerBase::StateControl(MotionControlInputs* pInputs, MotionControlOutputs* pOutputs)
{
	switch (mCurrSysState)
	{
	case eIdle:
		mCurrSysState = SystemState::eInitialization;
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
			for (int i = 0; i < kAxisNum; i++)
			{
				for (int j = 0; j < kJointNum; j++)
				{
					// update initial positions
					m_InitPos[i][j] = AxisInst[i][j].GetFeedbackPosition();
				}
			}

			mCurrSysState = SystemState::eDisabled;
		}
		break;

	case eDisabled:
		if (pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (MotorServoOn())
			{
				mCurrSysState = SystemState::eStandby;
			}
		}
		break;

	case eStandby:
		if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (MotorServoOff())
			{
				mCurrSysState = SystemState::eDisabled;
			}
		}
		else if (pInputs->PanelInformation.AxisEnable[0] || pInputs->PanelInformation.AxisEnable[1] || pInputs->PanelInformation.AxisEnable[2] || pInputs->PanelInformation.AxisEnable[3])// check whether axis button is pressed 
		{
			// Reset m_HandwheelInitPos upon entering eHandwheel state
			memcpy(&m_HandwheelInitPos, &m_FdbPos, sizeof(m_FdbPos));
			mCurrSysState = SystemState::eHandwheel;
		}
		else if (pInputs->mockButton[0]) // moving, button status from host application
		{
			mCurrSysState = SystemState::eMoving;
		}
		else if (pInputs->mockButton[2]) // test, button status from host application
		{
			mCurrSysState = SystemState::eTest;
		}
		else if (pInputs->mockButton[4] || pInputs->PanelInformation.Estop) // Estop, button status from host application or panel
		{
			mCurrSysState = SystemState::eEmergency;
		}
		else
		{
			StateStandby();
		}
		break;

	case eMoving:
		if (pInputs->mockButton[0])
		{
			StateMoving(pInputs);
		}
		else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (MotorServoOff()){
				mCurrSysState = SystemState::eDisabled;
			}
		}
		else
		{
			mCurrSysState = SystemState::eStandby;
		}
		break;

	case eHandwheel:
		if (pInputs->PanelInformation.AxisEnable[0] || pInputs->PanelInformation.AxisEnable[1] || pInputs->PanelInformation.AxisEnable[2] || pInputs->PanelInformation.AxisEnable[3])
		{
			StateHandwheel(pInputs);
		}
		else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (MotorServoOff()) {
				mCurrSysState = SystemState::eDisabled;
			}
		}
		else
		{
			mCurrSysState = SystemState::eStandby;
		}
		break;

	case eFault:
		break;

	case eEmergency:
		if (pInputs->mockButton[4] || pInputs->PanelInformation.Estop) // Estop
		{
			mCurrSysState = SystemState::eEmergency;
		}
		else
		{
			mCurrSysState = SystemState::eDisabled;
		}
		break;

	case eTest:
		if (pInputs->mockButton[2])
		{
			StateTest(pInputs, pOutputs);
		}
		else if (!pInputs->PanelInformation.ServoOn) // check whether servo-on button is pressed 
		{
			if (MotorServoOff()) {
				mCurrSysState = SystemState::eDisabled;
			}
		}
		else
		{
			mCurrSysState = SystemState::eStandby;
		}
		break;

	default:
		break;
	}

	// Observe current system state
	mSysStateMachine = mCurrSysState;
	pOutputs->TestOutputs[0] = static_cast<double>(mSysStateMachine);
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
			m_AxisParam[i][j].abs_dir		 = pInputs->MotionCtrlParam[i][j].AbsEncDir;
			m_AxisParam[i][j].abs_encoder_res = pInputs->MotionCtrlParam[i][j].AbsEncRes;
			m_AxisParam[i][j].abs_zero_pos	 = pInputs->MotionCtrlParam[i][j].AbsZeroPos;
										 
			m_AxisParam[i][j].rated_curr		 = pInputs->MotionCtrlParam[i][j].RatedCurrent;
			m_AxisParam[i][j].rated_tor		 = pInputs->MotionCtrlParam[i][j].RatedTorque;
			m_AxisParam[i][j].tor_dir		 = pInputs->MotionCtrlParam[i][j].TorDir;
			
			// Add this to avoid 'SSE invalid Operation' since variable rated_curr may be zero at first and denominator should not be zero
			if (m_AxisParam[i][j].rated_curr > kEPSILON)
			{
				m_AxisParam[i][j].tor_cons = m_AxisParam[i][j].rated_tor / m_AxisParam[i][j].rated_curr;
			}
											 
			m_AxisParam[i][j].abs_pos_ub		 = pInputs->MotionCtrlParam[i][j].PosUpperLimit;
			m_AxisParam[i][j].abs_pos_lb		 = pInputs->MotionCtrlParam[i][j].PosLowerLimit;
			m_AxisParam[i][j].reduction_ratio = pInputs->MotionCtrlParam[i][j].ReductionRatio;

			m_AxisParam[i][j].tor_pdo_max	 = pInputs->MotionCtrlParam[i][j].TorPdoMax;
			m_AxisParam[i][j].abs_encoder_type = pInputs->MotionCtrlParam[i][j].AbsEncType;
			
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
bool CControllerBase::MotorServoOn()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			if (!AxisInst[i][j].ServoOn())
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
bool CControllerBase::MotorServoOff()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			if (!AxisInst[i][j].ServoOff())
			{
				return false;
			}
		}
	}
	return true;
}

/**
 * @brief Action under standby state, hold current positions
 * 
 */
void CControllerBase::StateStandby()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].OperationMode = CSP;
			m_CmdPos[i][j] = m_FdbPos[i][j];
		}
	}
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
			AxisInst[i][j].OperationMode = pInputs->CommandData.opMode[i];

			m_CmdPos[i][j] = pInputs->CommandData.pos[i];

			m_CmdVel[i][j] = pInputs->CommandData.vel[i];

			m_CmdTor[i][j] = KmatrixController(m_FdbPos[i][j], m_FdbVel[i][j], m_FdbAcc[i][j],
				pInputs->CommandData.pos[i], pInputs->CommandData.vel[i], pInputs->CommandData.acc[i],
				pInputs->CommandData.kp[i], pInputs->CommandData.kv[i], pInputs->CommandData.ka[i]);
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
			AxisInst[i][j].OperationMode = CSP;
			m_CmdPos[i][j] = m_HandwheelInitPos[i][j] + pInputs->PanelInformation.Handwheel_dPos[i];
		}
	}
}

/**
 * @brief Action under emergency state, currently hold positions for simply usage, may change strategy in the future
 * 
 */
void CControllerBase::StateEmergencyStop()
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			AxisInst[i][j].OperationMode = CSP;
			m_CmdPos[i][j] = m_FdbPos[i][j];
		}
	}
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
		break;
	case 1:
		TestCSP(pInputs->TestInputs[1]);
		break;
	case 2:
		TestCST(pInputs->TestInputs[2]);
		break;
	case 3:
		TestCSV(pInputs->TestInputs[3]);
		break;
	case 4:
		TestMoveSin(pInputs);
		break;
	case 10:
		TestBrake();
		break;
	default:
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
			AxisInst[i][j].OperationMode = CST;

			m_CmdTor[i][j] = KmatrixController(m_FdbPos[i][j], m_FdbVel[i][j], m_FdbAcc[i][j],
				pInputs->CommandData.pos[i], pInputs->CommandData.vel[i], pInputs->CommandData.acc[i],
				pInputs->CommandData.kp[i], pInputs->CommandData.kv[i], pInputs->CommandData.ka[i]);
		}
	}
}

/**
 * @brief Test CSP
 * 
 * @param length incremental positions, unit mm
 */
void CControllerBase::TestCSP(double _length)
{
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdPos[i][j] = m_FdbPos[i][j] + _length;
			AxisInst[i][j].OperationMode = CSP;
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
			AxisInst[i][j].OperationMode = CSV;
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
	for (int i = 0; i < kAxisNum; i++)
	{
		for (int j = 0; j < kJointNum; j++)
		{
			m_CmdPos[i][j] = m_FdbPos[i][j];
			AxisInst[i][j].OperationMode = CSP;
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
			AxisInst[i][j].OperationMode = CST;
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