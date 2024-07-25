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
		return static_cast<double>(FdbPosVal - m_AxisParam.abs_zero_pos) / static_cast<double>(m_AxisParam.abs_encoder_res);
	}

	/**
	 * @brief Get velocity of encoder
	 * 
	 * @return double 
	 */
	double CAxis::GetFeedbackVelocity()
	{
		return static_cast<double>(FdbVelVal) / static_cast<double>(m_AxisParam.abs_encoder_res);
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
		return static_cast<double>(FdbTorVal);
	}

	/**
	 * @brief Send torque command to motor, unit is 1/1000;
	 * 
	 * @param _jnt_tor 
	 * @return short 
	 */
	short CAxis::SendTargetTorque(double _jnt_tor)
	{
		double tor_m = _jnt_tor / m_AxisParam.reduction_ratio;
		double tor_i = tor_m / m_AxisParam.tor_cons;

		short tor_pdo = static_cast<short>(tor_i * 1000 / m_AxisParam.rated_curr);
		tor_pdo = CLAMP(tor_pdo, -m_AxisParam.tor_pdo_max, m_AxisParam.tor_pdo_max);
		return tor_pdo;
	}

	/**
	 * @brief Send position command to motor
	 * 
	 * 
	 * @param _ref_pos 
	 * @return long 
	 */
	long CAxis::SendTargetPosition(double _ref_pos)
	{
		long reference_position = static_cast<long>(_ref_pos * m_AxisParam.abs_encoder_res);
		// add soft limit
		reference_position = CLAMP(reference_position, m_AxisParam.abs_pos_lb, m_AxisParam.abs_pos_ub);

		return reference_position;
	}

	/**
	 * @brief Return to zero position
	 *
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
	 * @param _ref_pos 
	 * @return long 
	 */
	long CAxis::SendTargetVelocity(double _ref_vel)
	{
		long reference_velocity = static_cast<long>(_ref_vel * m_AxisParam.abs_encoder_res);
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

			m_AxisParam.reduction_ratio = _mc_param.reduction_ratio;

			m_AxisParam.abs_encoder_type = _mc_param.abs_encoder_type;
			m_AxisParam.tor_pdo_max = _mc_param.tor_pdo_max;

			bInit = true;
		}
		return bInit;
	}

	/**
	 * @brief Set operation mode which is pre-set in driver manager
	 * 
	 * @param _mode 
	 * @return true 
	 * @return false 
	 */
	void CAxis::SetOperationMode(OpMode _mode)
	{
		if (_mode == CSP) // primary mode of operation -- pos control
		{
			ControlWord = MasterControlWord::eDriveOn_CSP;
		}
		else if (_mode == CST) // secondary mode -- torque control
		{
			ControlWord = MasterControlWord::eDriveOn_CST;
		}
		else if (_mode == CSV) // secondary mode -- vel control
		{
			ControlWord = MasterControlWord::eDriveOn_CSV;
		}
	}

	/**
	 * @brief Power off
	 * Need to be verified.
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::QuickStop()
	{
		bool quick_stop_flag = false;
		if (GetDriverState() == DriverState::eReadyToOperate)
		{
			ControlWord = MasterControlWord::eHaltDrive;
		}
		else if (GetDriverState() == DriverState::ePowerStageLocked)
		{
			quick_stop_flag = true;
		}
		return quick_stop_flag;
	}

	/**
	 * @brief Clear error or warning
	 * Need to be verified.
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::FaultReset()
	{
		bool reset_fault_flag = false;
		if (GetDriverState() == DriverState::eShutDownError)
		{
			ControlWord = MasterControlWord::eRestartDrive;
		}
		else
		{
			reset_fault_flag = true;
		}
		return reset_fault_flag;
	}

	/**
	 * @brief Servo On
	 * Search S-0-0134 master control word in https://infosys.beckhoff.com/index_en.htm for details of how to send ControlWord
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::ServoOn()
	{
		bool onFlag = false;
		if (GetDriverState() == DriverState::ePowerStageLocked)
		{
			ControlWord = MasterControlWord::eDriveOn_CSP;
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
	bool CAxis::ServoOff()
	{
		bool offFlag = false;
		ControlWord = MasterControlWord::eDriveOff;
		
		if (GetDriverState() == DriverState::ePowerStageLocked)
		{
			offFlag = true;
		}

		return offFlag;
	}

	/**
	 * @brief Check whether error or warning happens
	 * 
	 * @return true 
	 * @return false 
	 */
	bool CAxis::IsFaultState()
	{
		return (GetDriverState() == DriverState::eShutDownError);
	}

	/**
	 * @brief Return error code if error happened
	 * 
	 * @return unsigned int 
	 */
	unsigned int CAxis::CheckErrorCode()
	{
		return ErrorCode;
	}

	/**
	 * @brief Return warn code if warning happened
	 * 
	 * @return unsigned int 
	 */
	unsigned int CAxis::CheckWarnCode()
	{
		return WarningCode;
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
		return static_cast<DriverState>(StatusWord & DriverState::eReadyToOperate);
	}
}