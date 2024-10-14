#pragma once
#pragma hdrstop

#include "Driver.h"

namespace Driver
{
	CDriver::CDriver()
	{

	}

	CDriver::~CDriver()
	{

	}

	/**
	 * @brief Get position of absolute encoder
	 * 
	 * @return double 
	 */
	double CDriver::GetFeedbackPosition()
	{
		if (m_DriverParam->AbsEncRes > kEpsilon) // Avoid dividing zero error
		{
			return static_cast<double>(m_FdbPosVal - m_DriverParam->AbsZeroPos) / static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) / m_DriverParam->TransmissionRatio;
		}
		else
		{
			return 0.0;
		}
	}

	/**
	 * @brief Get velocity of encoder
	 * 
	 * @return double 
	 */
	double CDriver::GetFeedbackVelocity()
	{
		if (m_DriverParam->AbsEncRes > kEpsilon) // Avoid dividing zero error
		{
			return static_cast<double>(m_FdbVelVal) / static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) / m_DriverParam->TransmissionRatio;
		}
		else
		{
			return 0.0;
		}
	}

	/**
	 * @brief Get acceleration of encoder
	 * 
	 * @param _input 
	 * @param _first_derivative 
	 * @param _second_derivative 
	 */
	void CDriver::ComputeAcceleration(double _input, double* _first_derivative, double* _second_derivative)
	{
		// DiffProcess(_input, _first_derivative, _second_derivative);
	}

	/**
	 * @brief Get torque
	 * 
	 * @return double 
	 */
	double CDriver::GetFeedbackTorque()
	{
		return static_cast<double>(m_FdbTorVal) * m_DriverParam->TorDir;
	}

	/**
	 * @brief Send torque command to motor, unit is 1/1000;
	 * 
	 * Currently, the input for this function is a fraction of the rated torque, expressed in thousandths. 
	 * If the input is to be the actual torque in the future, further conversion will be required.
	 * 
	 * 'additive_tor' is used to counteract the effects of external forces, 
	 * such as the gravitational influence on the z-axis.
	 * 
	 * @param _cmd_tor 
	 */
	void CDriver::SetTargetTorque(double _cmd_tor)
	{
		/*double tor_m = _jnt_tor / m_DriverParam->reduction_ratio;
		double tor_i = tor_m / m_DriverParam->tor_cons;
		short tor_pdo = static_cast<short>(tor_i * 1000 / m_DriverParam->rated_curr * m_DriverParam->tor_dir);*/
		
		short tor_pdo = static_cast<short>(_cmd_tor) * m_DriverParam->TorDir + m_DriverParam->AdditiveTorque;
		m_TargetTor = CLAMP(tor_pdo, -m_DriverParam->TorPdoMax, m_DriverParam->TorPdoMax);
	}

	/**
	 * @brief Send position command to motor
	 * 
	 * @param _cmd_pos 
	 */
	void CDriver::SetTargetPosition(double _cmd_pos)
	{
		m_TargetPos = static_cast<long>(_cmd_pos * static_cast<double>(m_DriverParam->AbsEncRes)
			* static_cast<double>(m_DriverParam->AbsEncDir) * m_DriverParam->TransmissionRatio)
			+ m_DriverParam->AbsZeroPos;
	}

	/**
	 * @brief Return to zero position
	 *
	 */
	void CDriver::ReturnToZeroPosition()
	{
		m_TargetPos =  m_DriverParam->AbsZeroPos;
	}

	/**
	 * @brief Send velocity command to motor
	 * 
	 * 
	 * @param _cmd_vel
	 * @return long 
	 */
	void CDriver::SetTargetVelocity(double _cmd_vel)
	{
		m_TargetVel = static_cast<long>(_cmd_vel * static_cast<double>(m_DriverParam->AbsEncRes)
			* m_DriverParam->TransmissionRatio * m_DriverParam->AbsEncDir);
	}

	bool CDriver::SetDriverParam()
	{
		bool bLoadParam = false;

		if ((fabs_(m_DriverParam->AbsEncDir) > kEpsilon) && (fabs_(m_DriverParam->AbsEncRes) > kEpsilon) &&
			(fabs_(m_DriverParam->TorDir) > kEpsilon) && (fabs_(m_DriverParam->RatedCurrent) > kEpsilon) &&
			(fabs_(m_DriverParam->RatedTorque) > kEpsilon) && (fabs_(m_DriverParam->TorPdoMax) > kEpsilon))
		{
			bLoadParam = true;
		}

		// Double check to ensure successful initialization
		return bLoadParam && IsDriverReady();
	}

	/**
	 * @brief Set operation mode which is pre-set in driver manager
	 * 
	 * @param _mode
	 */
	void CDriver::SetOperationMode(OpMode _mode)
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			SetOperationMode_Beckhoff(_mode);
			break;
		case 1:
			SetOperationMode_General(_mode);
			break;
		}
	}

	void CDriver::SetOperationMode_Beckhoff(OpMode _mode)
	{
		m_OperationMode = _mode;
		if (_mode == OpMode::CSP) // primary mode of operation -- pos control
		{
			if (m_Flip)
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSP + MasterControlWord::eControlUnitSync;
				m_Flip = !m_Flip;
			}
			else
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSP;
				m_Flip = !m_Flip;
			}
		}
		else if (_mode == OpMode::CST) // secondary mode -- torque control
		{
			if (m_Flip)
			{
				m_ControlWord = MasterControlWord::eDriveOn_CST + MasterControlWord::eControlUnitSync;
				m_Flip = !m_Flip;
			}
			else
			{
				m_ControlWord = MasterControlWord::eDriveOn_CST;
				m_Flip = !m_Flip;
			}
		}
		else if (_mode == OpMode::CSV) // secondary mode -- vel control
		{
			if (m_Flip)
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSV + MasterControlWord::eControlUnitSync;
				m_Flip = !m_Flip;
			}
			else
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSV;
				m_Flip = !m_Flip;
			}
		}
	}

	void CDriver::SetOperationMode_General(OpMode _mode)
	{
		m_OperationMode = _mode;
		if (_mode == OpMode::CSP)
		{
			m_ControlWord = 15;  // use enum later
		}
		else if (_mode == OpMode::CST)
		{
			m_ControlWord = 15;
		}
		else if (_mode == OpMode::CSV)
		{
			m_ControlWord = 15;
		}
	}

	/**
	 * @brief Get actual operation mode by parsing the eighth to tenth bits of the status word 
	 * 
	 * 0 = main mode IDN S-0-0032
	 * 1 = secondary mode 1 IDN S-0-0033
	 * 2 = secondary mode 2 IDN S-0-0034
	 * 
	 * @return OpMode 
	 */
	OpMode CDriver::GetActualOperationMode()
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			m_ActualOpMode = m_OpModeMask & (m_StatusWord >> 8);
			switch (m_ActualOpMode)
			{
			case 0:
				return OpMode::CSP;
				break;
			case 1:
				return OpMode::CST;
				break;
			case 2:
				return OpMode::CSV;
				break;
			default:
				return OpMode::CSP;
				break;
			}
		case 1:
			return static_cast<OpMode>(m_ActualOpMode);
			break;
		}

	}

	void CDriver::ControlUnitSync()
	{
		// m_ControlWord = MasterControlWord::eControlUnitSync;
		switch (m_DriverParam->DriverType)
		{
		case 0:
			m_ControlWord = MasterControlWord::eControlUnitSync;
			break;
		case 1:
			//m_ControlWord = 0;
			ControlUnitSync_General();
			break;
		}
	}

	void CDriver::ControlUnitSync_General()
	{
		if (GetDriverState_General() == DriverState_General::eNotReadyToSwitchOn)
		{
			m_ControlWord = ControlWord_General::eDisableVoltage;
		}
		else if (GetDriverState_General() == DriverState_General::eSwitchOnDisabled
			|| GetDriverState_General() == DriverState_General::eSwitchOnDisabled + DriverState_General::eQuickStopBit)
		{
			m_ControlWord = ControlWord_General::eShutdown;
		}
		else if (GetDriverState_General() == DriverState_General::eReadyToSwitchOn)
		{
			m_ControlWord = ControlWord_General::eSwitchOn;
		}
		else if (GetDriverState_General() == DriverState_General::eDriverFault
			|| GetDriverState_General() == DriverState_General::eDriverFault + DriverState_General::eQuickStopBit)
		{
			m_ControlWord = ControlWord_General::eFaultReset;
		}
	}

	/**
	 * @brief Servo On
	 * Search S-0-0134 master control word in https://infosys.beckhoff.com/index_en.htm for details of how to send ControlWord
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CDriver::Enable()
	{
		if (!IsDriverShielded())
		{
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return Enable_Beckhoff();
				break;
			case 1:
				return Enable_General();
				break;
			}
		}
		else
		{
			return true;
		}
		// TODO : support more motor brands.
	}

	bool CDriver::Enable_Beckhoff()
	{
		bool onFlag = false;
		if (GetDriverState() == DriverState::ePowerStageLocked)
		{
			if (m_Flip)
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSP + MasterControlWord::eControlUnitSync;
				m_Flip = !m_Flip;
			}
			else
			{
				m_ControlWord = MasterControlWord::eDriveOn_CSP;
				m_Flip = !m_Flip;
			}
		}
		else if (GetDriverState() == DriverState::eReadyToOperate)
		{
			onFlag = true;
		}

		return onFlag;
	}

	// TODO
	bool CDriver::Enable_General()
	{
		bool onFlag = false;

		if (GetDriverState_General() == DriverState_General::eSwitchedOn)
		{
			m_ControlWord = ControlWord_General::eEnableOperation;
		}
		else if (GetDriverState_General() == DriverState_General::eOperationEnabled)
		{
			onFlag = true;
		}
		
		return onFlag;
	}

	/**
	 * @brief Servo Off 
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CDriver::Disable()
	{
		if (!IsDriverShielded())
		{
			// return Disable_Beckhoff();
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return Disable_Beckhoff();
				break;
			case 1:
				return Disable_General();
				break;
			}
		}
		else
		{
			return true;
		}
		// TODO : support more motor brands.
	}

	bool CDriver::Disable_Beckhoff()
	{
		bool offFlag = false;
		m_ControlWord = MasterControlWord::eDriveOff;

		if (GetDriverState() == DriverState::ePowerStageLocked)
		{
			offFlag = true;
		}

		return offFlag;
	}

	// TODO
	bool CDriver::Disable_General()
	{
		bool offFlag = true;

		m_ControlWord = ControlWord_General::eDisableOperation;
		//m_ControlWord = ControlWord_General::eShutdown;

		if (GetDriverState_General() == DriverState_General::eSwitchedOn)
		{
			offFlag = true;
		}

		return offFlag;
	}

	/**
	 * @brief Check whether error happens
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CDriver::IsFaultState()
	{
		return (m_ErrorCode !=0);
	}

	bool CDriver::IsEmergencyState()
	{
		return (GetDriverState() == DriverState::eLogicReadyForPowerOn);
	}

	/**
	 * @brief Check if the position exceeds the soft limits.
	 *
	 * @return true
	 * @return false
	 */
	bool CDriver::IsSoftLimitExceeded()
	{
		if (SIGN(m_DriverParam->AbsEncDir) == 1)
		{
			return (m_FdbPosVal < m_DriverParam->PosLowerLimit || m_FdbPosVal > m_DriverParam->PosUpperLimit);
		}
		else
		{
			return (m_FdbPosVal > m_DriverParam->PosLowerLimit || m_FdbPosVal < m_DriverParam->PosUpperLimit);
		}
	}

	/**
	 * @brief Check if the position exceeds the hard limits.
	 * Normally, m_DigitalInput = 15(2#1111), 
	 * if optoelectronic switch connected to Channel 0 is blocked, m_DigitalInput = 14(2#1110); 
	 * if optoelectronic switch connected to Channel 1 is blocked, m_DigitalInput = 13(2#1101); 
	 * and so on.
	 * 
	 * @return true
	 * @return false
	 */
	bool CDriver::IsHardLimitExceeded()
	{
		return (m_DigitalInput == m_DigitalMask - (1 << m_DriverParam->NegativeHardBit)
			|| m_DigitalInput == m_DigitalMask - (1 << m_DriverParam->PositiveHardBit));
	}

	/**
	 * @brief Check if the position exceeds the hard or soft limits.
	 *
	 * @return true
	 * @return false
	 */
	bool CDriver::IsLimitExceeded()
	{
		if (!IsDriverShielded())
		{
			return (IsHardLimitExceeded() || IsSoftLimitExceeded());
		}
		else { return false; }
	}

	/**
	 * @brief Return error code if error happened
	 * 
	 * @return SHORT 
	 */
	short CDriver::CheckErrorCode()
	{
		return m_ErrorCode;
	}

	/**
	 * @brief Return warn code if warning happened
	 * 
	 * @return SHORT 
	 */
	short CDriver::CheckWarnCode()
	{
		return m_WarningCode;
	}

	bool CDriver::IsDriverReady()
	{
		if (!IsDriverShielded())
		{
			// return (GetDriverState() != DriverState::eNotReadyForPowerUp);

			switch (m_DriverParam->DriverType)
			{
			case 0:
				return (GetDriverState() != DriverState::eNotReadyForPowerUp);
				break;
			case 1:
				return (GetDriverState_General() != DriverState_General::eNotReadyToSwitchOn);
				break;
			}
		}
		else { return true; }
	}

	bool CDriver::IsDriverShielded()
	{
		return !(m_DriverParam->Exposed);
	}

	/**
	 * @brief Check whether motor is enabled
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CDriver::IsEnableState()
	{
		if (!IsDriverShielded())
		{
			return (GetDriverState() == DriverState::eReadyToOperate);
		}
		else { return true; }
	}

	/**
	 * @brief Check and return current state of driver
	 * Search S-0-0135 Drive status word in https://infosys.beckhoff.com/index_en.htm for details of StatusWord
	 * @return short 
	 */
	DriverState CDriver::GetDriverState()
	{
		return static_cast<DriverState>(m_StatusWord & DriverState::eReadyToOperate);
	}

	DriverState_General CDriver::GetDriverState_General()
	{
		return static_cast<DriverState_General>(m_StatusWord & m_DriverStateGeneralMask);
	}

	void CDriver::MapParameters(DriverInput* _DriverInput, DriverOutput* _DriverOutput, MotionControlInfo* _DriverParam)
	{
		m_DriverInput = _DriverInput;
		m_DriverOutput = _DriverOutput;
		m_DriverParam = _DriverParam;
	}

	void CDriver::Scan()
	{
		if (!IsDriverShielded())
		{
			m_StatusWord = m_DriverInput->StatusWord;

			m_FdbPosVal = m_DriverInput->ActualAbsPos;
			m_FdbVelVal = m_DriverInput->ActualVel;
			m_FdbTorVal = m_DriverInput->ActualTor;

			m_ErrorCode = m_DriverInput->ErrorCode;
			m_WarningCode = m_DriverInput->WarningCode;

			m_DigitalInput = m_DriverInput->DigitalInput_1;
			m_ActualOpMode = m_DriverInput->ActualOpMode;
		}
	}

	void CDriver::Actuate()
	{
		if (!IsDriverShielded())
		{
			m_DriverOutput->ControlWord = m_ControlWord;
			m_DriverOutput->TargetPosition = m_TargetPos;
			m_DriverOutput->TargetVelocity = m_TargetVel;
			m_DriverOutput->TargetTorque = m_TargetTor;
			m_DriverOutput->OperationMode = m_OperationMode;
		}

	}
}