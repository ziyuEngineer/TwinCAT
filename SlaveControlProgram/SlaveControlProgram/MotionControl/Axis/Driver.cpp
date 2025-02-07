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
			switch (m_DriverParam->MoveType)
			{
			case 0: // linear, unit: mm
				return static_cast<double>(m_FdbPosVal - m_DriverParam->AbsZeroPos) / static_cast<double>(m_DriverParam->AbsEncRes)
					* static_cast<double>(m_DriverParam->AbsEncDir) / m_DriverParam->TransmissionRatio;
				break;

			case 1: // rotate, unit: rad
				return static_cast<double>(m_FdbPosVal - m_DriverParam->AbsZeroPos) / static_cast<double>(m_DriverParam->AbsEncRes)
					* static_cast<double>(m_DriverParam->AbsEncDir) * (2.0 * PI) / m_DriverParam->TransmissionRatio;
				break;
			}
		}
		else
		{
			return 0.0;
		}
	}

	/**
	 * @brief Get effective position command
	 *
	 * @return double
	 */
	double CDriver::GetEffectivePositionCommand()
	{
		if (m_DriverParam->AbsEncRes > kEpsilon) // Avoid dividing zero error
		{
			switch (m_DriverParam->MoveType)
			{
			case 0: // linear
				return static_cast<double>(m_EffectivePosCmd - m_DriverParam->AbsZeroPos) / static_cast<double>(m_DriverParam->AbsEncRes)
					* static_cast<double>(m_DriverParam->AbsEncDir) / m_DriverParam->TransmissionRatio;
				break;
			case 1: // rotate
				return static_cast<double>(m_EffectivePosCmd - m_DriverParam->AbsEncRes) / static_cast<double>(m_DriverParam->AbsEncRes)
					* static_cast<double>(m_DriverParam->AbsEncDir) * (2.0 * PI) / m_DriverParam->TransmissionRatio;
				break;
			}
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
	 * @brief Get effective velocity command
	 *
	 * @return double
	 */
	double CDriver::GetEffectiveVelocityCommand()
	{
		if (m_DriverParam->AbsEncRes > kEpsilon) // Avoid dividing zero error
		{
			return static_cast<double>(m_EffectiveVelCmd) / static_cast<double>(m_DriverParam->AbsEncRes)
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
	void CDriver::ComputeAcceleration(double input, double* first_derivative, double* second_derivative)
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
	 * @brief Get effective torque command
	 *
	 * @return double
	 */
	double CDriver::GetEffectiveTorqueCommand()
	{
		return static_cast<double>(m_EffectiveTorCmd) * m_DriverParam->TorDir;
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
	void CDriver::SetTargetTorque(double cmd_tor)
	{
		short tor_pdo = static_cast<short>(cmd_tor) * m_DriverParam->TorDir + m_DriverParam->AdditiveTorque;
		m_TargetTor = CLAMP(tor_pdo, -m_DriverParam->TorPdoMax, m_DriverParam->TorPdoMax);
	}

	void CDriver::SetAdditiveTorque(double additive_tor)
	{
		m_AdditiveTor = static_cast<short>(additive_tor) * m_DriverParam->TorDir;
	}

	/**
	 * @brief Send position command to motor
	 * 
	 * @param _cmd_pos 
	 */
	void CDriver::SetTargetPosition(double cmd_pos)
	{
		switch (m_DriverParam->MoveType)
		{
		case 0: // linear
			m_TargetPos = static_cast<long>(cmd_pos * static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) * m_DriverParam->TransmissionRatio)
				+ m_DriverParam->AbsZeroPos;
			break;

		case 1: // rotate
			m_TargetPos = static_cast<long>(cmd_pos / (2.0 * PI) * static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) * m_DriverParam->TransmissionRatio);
			break;
		}
	}

	void CDriver::SetAdditivePosition(double additive_pos)
	{
		switch (m_DriverParam->MoveType)
		{
		case 0: // linear
			m_AdditivePos = static_cast<long>(additive_pos * static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) * m_DriverParam->TransmissionRatio);
			break;

		case 1: // rotate
			m_AdditivePos = static_cast<long>(additive_pos / (2.0 * PI) * static_cast<double>(m_DriverParam->AbsEncRes)
				* static_cast<double>(m_DriverParam->AbsEncDir) * m_DriverParam->TransmissionRatio);
			break;
		}
	}

	/**
	 * @brief Send velocity command to motor
	 * 
	 * 
	 * @param _cmd_vel
	 * @return long 
	 */
	void CDriver::SetTargetVelocity(double cmd_vel)
	{
		m_TargetVel = static_cast<long>(cmd_vel * static_cast<double>(m_DriverParam->AbsEncRes)
			* m_DriverParam->TransmissionRatio * m_DriverParam->AbsEncDir);
	}

	void CDriver::SetAdditiveVelocity(double additive_vel)
	{
		m_AdditiveVel = static_cast<long>(additive_vel * static_cast<double>(m_DriverParam->AbsEncRes)
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
	void CDriver::SetOperationMode(OpMode mode)
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			SetOperationMode_Beckhoff(mode);
			break;
		case 1:
			SetOperationMode_General(mode);
			break;
		}
	}

	void CDriver::SetOperationMode_Beckhoff(OpMode mode)
	{
		m_OperationMode = mode;
		if (mode == OpMode::CSP) // primary mode of operation -- pos control
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
		else if (mode == OpMode::CST) // secondary mode -- torque control
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
		else if (mode == OpMode::CSV) // secondary mode -- vel control
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

	void CDriver::SetOperationMode_General(OpMode mode)
	{
		m_OperationMode = mode;
		if (mode == OpMode::CSP)
		{
			m_ControlWord = ControlWordGeneral::eEnableOperation;
		}
		else if (mode == OpMode::CST)
		{
			m_ControlWord = ControlWordGeneral::eEnableOperation;
		}
		else if (mode == OpMode::CSV)
		{
			m_ControlWord = ControlWordGeneral::eEnableOperation;
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

	DriverStatus CDriver::GetActualDriverStatus()
	{
		if (!IsDriverShielded())
		{
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return GetDriverStatus_Beckhoff();
				break;
			case 1:
				return GetDriverStatus_General();
				break;
			}
		}
		else
		{
			return DriverStatus::DriverWorking;
		}
	}

	DriverStatus CDriver::GetDriverStatus_Beckhoff()
	{
		if (IsFaultState() || GetDriverState_Beckhoff() == DriverState::eShutDownError)
		{
			return DriverStatus::DriverFault;
		}
		else if (GetDriverState_Beckhoff() == DriverState::eReadyToOperate)
		{
			return DriverStatus::DriverWorking;
		}
		else
		{
			return DriverStatus::DriverDisabled;
		}
	}

	DriverStatus CDriver::GetDriverStatus_General()
	{
		if (IsFaultState() || GetDriverState_General() == DriverStateGeneral::eDriverFault
			|| GetDriverState_General() == DriverStateGeneral::eFaultReactionActive)
		{
			return DriverStatus::DriverFault;
		}
		else if (GetDriverState_General() == DriverStateGeneral::eOperationEnabled)
		{
			return DriverStatus::DriverWorking;
		}
		else
		{
			return DriverStatus::DriverDisabled;
		}
	}

	void CDriver::ControlUnitSync()
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			m_ControlWord = MasterControlWord::eControlUnitSync;
			break;
		case 1:
			ControlUnitSync_General();
			break;
		}
	}

	void CDriver::ControlUnitSync_General()
	{
		if (GetDriverState_General() == DriverStateGeneral::eNotReadyToSwitchOn)
		{
			m_ControlWord = ControlWordGeneral::eDisableVoltage;
		}
		else if (GetDriverState_General() == DriverStateGeneral::eSwitchOnDisabled
			|| GetDriverState_General() == DriverStateGeneral::eSwitchOnDisabled + DriverStateGeneral::eQuickStopBit)
		{
			m_ControlWord = ControlWordGeneral::eShutdown;
		}
		else if (GetDriverState_General() == DriverStateGeneral::eReadyToSwitchOn)
		{
			m_ControlWord = ControlWordGeneral::eSwitchOn;
		}
		else if (GetDriverState_General() == DriverStateGeneral::eDriverFault
			|| GetDriverState_General() == DriverStateGeneral::eDriverFault + DriverStateGeneral::eQuickStopBit)
		{
			m_ControlWord = ControlWordGeneral::eFaultReset;
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
		bool on_flag = false;
		if (GetDriverState_Beckhoff() == DriverState::ePowerStageLocked)
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
		else if (GetDriverState_Beckhoff() == DriverState::eReadyToOperate)
		{
			on_flag = true;
		}

		return on_flag;
	}

	bool CDriver::Enable_General()
	{
		bool on_flag = false;

		if (GetDriverState_General() == DriverStateGeneral::eSwitchedOn)
		{
			m_ControlWord = ControlWordGeneral::eEnableOperation;
		}
		else if (GetDriverState_General() == DriverStateGeneral::eOperationEnabled)
		{
			on_flag = true;
		}
		
		return on_flag;
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
		bool off_flag = false;
		m_ControlWord = MasterControlWord::eDriveOff;

		if (GetDriverState_Beckhoff() == DriverState::ePowerStageLocked)
		{
			off_flag = true;
		}

		return off_flag;
	}

	bool CDriver::Disable_General()
	{
		bool off_flag = false;

		m_ControlWord = ControlWordGeneral::eDisableOperation;

		if (GetDriverState_General() == DriverStateGeneral::eSwitchedOn)
		{
			off_flag = true;
		}

		return off_flag;
	}

	void CDriver::ClearError()
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			ClearError_Beckhoff();
			break;
		case 1:
			ClearError_General();
			break;
		}
	}

	void CDriver::ClearError_Beckhoff()
	{
		Enable_Beckhoff();
	}

	void CDriver::ClearError_General()
	{
		if (GetDriverState_General() == DriverStateGeneral::eDriverFault
			|| GetDriverState_General() == DriverStateGeneral::eDriverFault + DriverStateGeneral::eQuickStopBit)
		{
			m_ControlWord = ControlWordGeneral::eFaultReset;
		}
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
		return (GetDriverState_Beckhoff() == DriverState::eLogicReadyForPowerOn);
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

	void CDriver::QuickStop()
	{
		switch (m_DriverParam->DriverType)
		{
		case 0:
			//ClearError_Beckhoff();
			break;
		case 1:
			m_ControlWord = ControlWordGeneral::eQuickStop;
			break;
		}
	}

	bool CDriver::IsDriverReady()
	{
		if (!IsDriverShielded())
		{
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return (GetDriverState_Beckhoff() != DriverState::eNotReadyForPowerUp);
				break;
			case 1:
				return (GetDriverState_General() != DriverStateGeneral::eNotReadyToSwitchOn);
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
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return (GetDriverState_Beckhoff() == DriverState::eReadyToOperate);
				break;
			case 1:
				return (GetDriverState_General() == DriverStateGeneral::eOperationEnabled);
				break;
			}
		}
		else { return true; }
	}

	bool CDriver::IsDisableState()
	{
		if (!IsDriverShielded())
		{
			switch (m_DriverParam->DriverType)
			{
			case 0:
				return (GetDriverState_Beckhoff() == DriverState::ePowerStageLocked);
				break;
			case 1:
				return (GetDriverState_General() == DriverStateGeneral::eReadyToSwitchOn);
				break;
			}
		}
		else { return true; }
	}

	bool CDriver::IsOpModeSwitched()
	{
		if (!IsDriverShielded())
		{
			return GetActualOperationMode() == m_OperationMode;
		}
		else { return true; }
	}

	/**
	 * @brief Check and return current state of driver
	 * Search S-0-0135 Drive status word in https://infosys.beckhoff.com/index_en.htm for details of StatusWord
	 * @return short 
	 */
	DriverState CDriver::GetDriverState_Beckhoff()
	{
		//return static_cast<DriverState>(m_StatusWord & DriverState::eReadyToOperate);
		return static_cast<DriverState>(m_StatusWord & m_DriverStateBeckhoffMask);
	}

	DriverStateGeneral CDriver::GetDriverState_General()
	{
		return static_cast<DriverStateGeneral>(m_StatusWord & m_DriverStateGeneralMask);
	}

	void CDriver::MapParameters(DriverInput* driver_input, DriverOutput* driver_output, const MotionControlInfo* driver_param)
	{
		m_DriverInput = driver_input;
		m_DriverOutput = driver_output;
		m_DriverParam = driver_param;
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
			//m_WarningCode = m_DriverInput->WarningCode;

			m_ActualOpMode = m_DriverInput->ActualOpMode;

			m_EffectivePosCmd = m_DriverInput->EffectivePositionCmd;
			m_EffectiveVelCmd = m_DriverInput->EffectiveVelocityCmd;
			m_EffectiveTorCmd = m_DriverInput->EffectiveTorqueCmd;
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
			m_DriverOutput->AdditiveTorque = m_AdditiveTor;
			m_DriverOutput->AdditivePosition = m_AdditivePos;
			m_DriverOutput->AdditiveVelocity = m_AdditiveVel;
		}
	}
}