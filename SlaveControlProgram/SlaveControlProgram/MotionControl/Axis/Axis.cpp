#pragma once
#pragma hdrstop

#include "Axis.h"

namespace Axis
{
	CAxis::CAxis()
	{
		
	}

	CAxis::~CAxis()
	{

	}

	/**
	 * @brief Get position of absolute encoder
	 * 
	 * @return double 
	 */
	double CAxis::GetFeedbackPosition()
	{
		return static_cast<double>(m_FdbPosVal - m_AxisParam.abs_zero_pos) / static_cast<double>(m_AxisParam.abs_encoder_res)
				* static_cast<double>(m_AxisParam.abs_dir) / m_AxisParam.transmission_ratio;
	}

	/**
	 * @brief Get velocity of encoder
	 * 
	 * @return double 
	 */
	double CAxis::GetFeedbackVelocity()
	{
		return static_cast<double>(m_FdbVelVal) / static_cast<double>(m_AxisParam.abs_encoder_res)
			* static_cast<double>(m_AxisParam.abs_dir) / m_AxisParam.transmission_ratio;
	}

	/**
	 * @brief Get acceleration of encoder
	 * 
	 * @param _input 
	 * @param _first_derivative 
	 * @param _second_derivative 
	 */
	void CAxis::ComputeAcceleration(double _input, double* _first_derivative, double* _second_derivative)
	{
		DiffProcess(_input, _first_derivative, _second_derivative);
	}

	/**
	 * @brief Get torque
	 * 
	 * @return double 
	 */
	double CAxis::GetFeedbackTorque()
	{
		return static_cast<double>(m_FdbTorVal) * m_AxisParam.tor_dir;
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
	 * @return short 
	 */
	short CAxis::SendTargetTorque(double _cmd_tor)
	{
		/*double tor_m = _jnt_tor / m_AxisParam.reduction_ratio;
		double tor_i = tor_m / m_AxisParam.tor_cons;
		short tor_pdo = static_cast<short>(tor_i * 1000 / m_AxisParam.rated_curr * m_AxisParam.tor_dir);*/
		
		short tor_pdo = static_cast<short>(_cmd_tor) * m_AxisParam.tor_dir + m_AxisParam.additive_tor;
		tor_pdo = CLAMP(tor_pdo, -m_AxisParam.tor_pdo_max, m_AxisParam.tor_pdo_max);
		return tor_pdo;
	}

	/**
	 * @brief Send position command to motor
	 * 
	 * @param _cmd_pos 
	 * @return long 
	 */
	long CAxis::SendTargetPosition(double _cmd_pos)
	{
		long reference_position = static_cast<long>(_cmd_pos * static_cast<double>(m_AxisParam.abs_encoder_res)
			* static_cast<double>(m_AxisParam.abs_dir) * m_AxisParam.transmission_ratio)
			+ m_AxisParam.abs_zero_pos;

		return reference_position;
	}

	/**
	 * @brief Return to zero position
	 *
	 * @return long
	 */
	long CAxis::ReturnToZeroPosition()
	{
		return m_AxisParam.abs_zero_pos;
	}

	/**
	 * @brief Send velocity command to motor
	 * 
	 * 
	 * @param _cmd_vel
	 * @return long 
	 */
	long CAxis::SendTargetVelocity(double _cmd_vel)
	{
		long reference_velocity = static_cast<long>(_cmd_vel * static_cast<double>(m_AxisParam.abs_encoder_res)
			* m_AxisParam.transmission_ratio * m_AxisParam.abs_dir);
		return reference_velocity;
	}

	/**
	 * @brief Set parameters of single motor, all parameters should have specific values which is unequal to zero 
	 * 
	 * @param _mc_param 
	 * @return true : successful initialization
	 * @return false 
	 */
	bool CAxis::SetAxisParam(AxisParam _mc_param)
	{
		bool bInit = false;

		if (!IsDriverReady())
		{
			return bInit;
		}

		if ((fabs_(_mc_param.abs_dir) > kEPSILON) && (fabs_(_mc_param.abs_encoder_res) > kEPSILON) &&
			(fabs_(_mc_param.tor_dir * _mc_param.tor_cons) > kEPSILON) && (fabs_(_mc_param.rated_curr) > kEPSILON) &&
			(fabs_(_mc_param.rated_tor) > kEPSILON) && (fabs_(_mc_param.tor_pdo_max) > kEPSILON))
		{
			m_AxisParam.abs_dir = SIGN(_mc_param.abs_dir);
			m_AxisParam.abs_encoder_res = _mc_param.abs_encoder_res;
			m_AxisParam.abs_zero_pos = _mc_param.abs_zero_pos;
			m_AxisParam.abs_pos_ub = _mc_param.abs_pos_ub;
			m_AxisParam.abs_pos_lb = _mc_param.abs_pos_lb;

			m_AxisParam.tor_dir = _mc_param.tor_dir;
			m_AxisParam.rated_curr = _mc_param.rated_curr;
			m_AxisParam.rated_tor = _mc_param.rated_tor;
			m_AxisParam.tor_cons = _mc_param.tor_dir * _mc_param.tor_cons;

			m_AxisParam.transmission_ratio = _mc_param.transmission_ratio;

			m_AxisParam.abs_encoder_type = _mc_param.abs_encoder_type;
			m_AxisParam.tor_pdo_max = _mc_param.tor_pdo_max;
			m_AxisParam.additive_tor = _mc_param.additive_tor;

			m_AxisParam.positive_hard_bit = _mc_param.positive_hard_bit;
			m_AxisParam.negative_hard_bit = _mc_param.negative_hard_bit;

			bInit = true;
		}
		return bInit;
	}

	/**
	 * @brief Set operation mode which is pre-set in driver manager
	 * 
	 * @param _mode
	 */
	void CAxis::SetOperationMode(OpMode _mode)
	{
		m_OperationMode = _mode;
		if (_mode == CSP) // primary mode of operation -- pos control
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
		else if (_mode == CST) // secondary mode -- torque control
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
		else if (_mode == CSV) // secondary mode -- vel control
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

	/**
	 * @brief Get actual operation mode by parsing the eighth to tenth bits of the status word 
	 * 
	 * 0 = main mode IDN S-0-0032
	 * 1 = secondary mode 1 IDN S-0-0033
	 * 2 = secondary mode 2 IDN S-0-0034
	 * 
	 * @return OpMode 
	 */
	OpMode CAxis::GetActualOperationMode()
	{
		unsigned int parsed_mode = m_OpModeMask & (m_StatusWord >> 8);
		switch(parsed_mode)
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

	}

	void CAxis::ControlUnitSync()
	{
		m_ControlWord = MasterControlWord::eControlUnitSync;
	}

	/**
	 * @brief Servo On
	 * Search S-0-0134 master control word in https://infosys.beckhoff.com/index_en.htm for details of how to send ControlWord
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::Enable()
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

	/**
	 * @brief Servo Off 
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::Disable()
	{
		bool offFlag = false;
		m_ControlWord = MasterControlWord::eDriveOff;
		
		if (GetDriverState() == DriverState::ePowerStageLocked)
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
	bool CAxis::IsFaultState()
	{
		return (m_ErrorCode !=0);
	}

	bool CAxis::IsEmergencyState()
	{
		return (GetDriverState() == DriverState::eLogicReadyForPowerOn);
	}

	/**
	 * @brief Check if the position exceeds the soft limits.
	 *
	 * @return true
	 * @return false
	 */
	bool CAxis::IsSoftLimitExceeded()
	{
		if (SIGN(m_AxisParam.abs_dir) == 1)
		{
			return (m_FdbPosVal < m_AxisParam.abs_pos_lb || m_FdbPosVal > m_AxisParam.abs_pos_ub);
		}
		else
		{
			return (m_FdbPosVal > m_AxisParam.abs_pos_lb || m_FdbPosVal < m_AxisParam.abs_pos_ub);
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
	bool CAxis::IsHardLimitExceeded()
	{
		return (m_DigitalInput == m_DigitalMask - (1 << m_AxisParam.negative_hard_bit)
				|| m_DigitalInput == m_DigitalMask - (1 << m_AxisParam.positive_hard_bit));
	}

	/**
	 * @brief Check if the position exceeds the hard or soft limits.
	 *
	 * @return true
	 * @return false
	 */
	bool CAxis::IsLimitExceeded()
	{
		return (IsHardLimitExceeded() || IsSoftLimitExceeded());
	}

	/**
	 * @brief Return error code if error happened
	 * 
	 * @return SHORT 
	 */
	short CAxis::CheckErrorCode()
	{
		return m_ErrorCode;
	}

	/**
	 * @brief Return warn code if warning happened
	 * 
	 * @return SHORT 
	 */
	short CAxis::CheckWarnCode()
	{
		return m_WarningCode;
	}

	bool CAxis::IsDriverReady()
	{
		return (GetDriverState() != DriverState::eNotReadyForPowerUp);
	}

	/**
	 * @brief Check whether motor is enabled
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::IsEnableState()
	{
		return (GetDriverState() == DriverState::eReadyToOperate);
	}

	/**
	 * @brief Check and return current state of driver
	 * Search S-0-0135 Drive status word in https://infosys.beckhoff.com/index_en.htm for details of StatusWord
	 * @return short 
	 */
	DriverState CAxis::GetDriverState()
	{
		return static_cast<DriverState>(m_StatusWord & DriverState::eReadyToOperate);
	}
}