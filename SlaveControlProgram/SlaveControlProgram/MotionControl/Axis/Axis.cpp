#pragma once
#pragma hdrstop

#include "Axis.h"

void CAxis::MapParameters(DriverInput* driver_input, DriverOutput* driver_output, const MotionControlInfo* driver_param, const InterpolationParameter* interpolation_parameter)
{
	m_Driver.MapParameters(driver_input, driver_output, driver_param);
	m_InterpolationParam = interpolation_parameter;
}

bool CAxis::PostConstruction()
{
	bool is_post_constructed = false;

	is_post_constructed = (m_InterpolationParam->CycleTime > kEpsilon && fabs_(m_InterpolationParam->MaxVelocity) > kEpsilon
		&& fabs_(m_InterpolationParam->MaxAcceleration) > kEpsilon && fabs_(m_InterpolationParam->MaxJerk) > kEpsilon);

	m_PositionInterpolator.SetCycleTime(m_InterpolationParam->CycleTime); // in [s]
	m_PositionInterpolator.SetMaxVelocity(m_InterpolationParam->MaxVelocity);
	m_PositionInterpolator.SetMaxAcceleration(m_InterpolationParam->MaxAcceleration);
	m_PositionInterpolator.SetMaxJerk(m_InterpolationParam->MaxJerk);

	m_VelocityInterpolator.SetCycleTime(m_InterpolationParam->CycleTime);
	m_VelocityInterpolator.SetMaxVelocity(m_InterpolationParam->MaxVelocity);
	m_VelocityInterpolator.SetMaxAcceleration(m_InterpolationParam->MaxAcceleration);
	m_VelocityInterpolator.SetMaxJerk(m_InterpolationParam->MaxJerk);

	return is_post_constructed;
}

void CAxis::Input()
{
	// Retrieve driver input data.
	m_Driver.Scan();

	// Update member variables
	m_FdbPos = m_Driver.GetFeedbackPosition();
	m_FdbPosFiltered = m_FdbPosFilter.process(m_FdbPos);
	m_FdbVel = m_Driver.GetFeedbackVelocity();
	m_FdbVelFiltered = m_FdbVelFilter.process(m_FdbVel);
	m_FdbTor = m_Driver.GetFeedbackTorque();
	m_ActualOpMode = m_Driver.GetActualOperationMode();
	m_CurrentDriverStatus = m_Driver.GetActualDriverStatus();
}

void CAxis::Output()
{
	switch (m_Driver.m_OperationMode)
	{
	case OpMode::CSP:
		//m_Driver.SetTargetPosition(m_CmdPos);
		break;
	case OpMode::CST:
		m_Driver.SetTargetTorque(m_CmdTor);
		break;
	case OpMode::CSV:
		m_Driver.SetTargetVelocity(m_CmdVel);
		break;
	default:
		m_Driver.ControlUnitSync();
		break;
	}

	// Update position command every cycle to avoid F324 Error 
	m_Driver.SetTargetPosition(m_CmdPos);

	// Update output data to driver
	m_Driver.Actuate();
}

bool CAxis::Initialize()
{
	return m_Driver.SetDriverParam();
}

bool CAxis::Enable()
{
	HoldPosition();
	return m_Driver.Enable();
}

bool CAxis::Disable()
{
	return m_Driver.Disable();
}

void CAxis::SwitchOperationMode(OpMode mode)
{
	switch (mode)
	{
	case OpMode::CSP:
		m_CmdPos = m_FdbPos;
		break;

	case OpMode::CSV:
		m_CmdVel = 0.0;
		break;

	case OpMode::CST:
		m_CmdTor = 0; // Need further consideration, set value to additive torque.
		break;
	}
	m_Driver.SetOperationMode(mode);
}

bool CAxis::IsOpModeSwitched()
{
	return m_Driver.IsOpModeSwitched();
}

bool CAxis::IsEnabled()
{
	return m_Driver.IsEnableState();
}

bool CAxis::IsDisabled()
{
	return m_Driver.IsDisableState();
}

void CAxis::HoldPosition()
{
	if (!m_IsStandbyPosSet)
	{
		m_StandbyPos = m_FdbPos;
		m_IsStandbyPosSet = true;
	}
	else 
	{
		m_Driver.SetOperationMode(OpMode::CSP);
		m_CmdPos = m_StandbyPos;
	}
}

void CAxis::StandStill()
{
	switch (m_ActualOpMode)
	{
	case OpMode::CSP:
		if (!m_IsStandbyPosSet)
		{
			m_StandbyPos = m_FdbPos;
			m_IsStandbyPosSet = true;
		}
		else
		{
			m_CmdPos = m_StandbyPos;
		}
		break;

	case OpMode::CSV:
		m_CmdVel = 0.0;
		UpdatePositionCommand();
		break;

	case OpMode::CST:
		m_CmdTor = 0; // Need further consideration, set value to additive torque.
		UpdatePositionCommand();
		break;
	}
	m_Driver.SetOperationMode(m_ActualOpMode);
}

void CAxis::Move(double cmd, OpMode mode, bool is_interpolated, bool update_feedback)
{
	m_Driver.SetOperationMode(mode);

	switch (mode)
	{
	case OpMode::CSP:
		if (!is_interpolated)
		{
			m_CmdPos = cmd;
		}
		else
		{
			m_CmdPosBeforeInterpolated = cmd;
			if (update_feedback)
			{
				m_CurrentStatePos.position = m_FdbPosFiltered;
				m_CurrentStatePos.velocity = m_FdbVelFiltered;
			}

			m_PositionInterpolator.SetTargetState({ m_CmdPosBeforeInterpolated, 0.0, 0.0 });

			if ((fabs_(cmd - m_CmdPos) < m_InterpolationParam->Tolerance))
			{
				m_CmdPos = cmd;
				m_IsPosInterpolationFinished = false;
			}
			else if (!m_IsPosInterpolationFinished)
			{
				PositionInterpolator1D::State next_state;
				double temp_Time;
				const auto result = m_PositionInterpolator.Update(m_CurrentStatePos, next_state, temp_Time);
				m_CmdPos = next_state.position;
				//m_CurrentStatePos.acceleration = next_state.acceleration;
				m_CurrentStatePos = next_state;

				m_IsPosInterpolationFinished = (result == ruckig::Result::Finished);
			}
		}
		break;

	case OpMode::CST:
		m_CmdTor = cmd;
		UpdatePositionCommand();
		break;

	case OpMode::CSV:
		if (!is_interpolated)
		{
			m_CmdVel = cmd;
		}
		else
		{
			m_CmdVelBeforeInterpolated = cmd;
			m_VelocityInterpolator.SetTargetState({ 0.0, m_CmdVelBeforeInterpolated, 0.0 });
			m_CurrentStateVel.velocity = m_FdbVelFiltered;

			if ((fabs_(cmd - m_CmdVel) < m_InterpolationParam->Tolerance))
			{
				m_CmdVel = cmd;
				m_IsVelInterpolationFinished = false; // reset for next interpolation
			}
			else if(!m_IsVelInterpolationFinished)
			{
				VelocityInterpolator1D::State next_state;
				double temp_Time;
				const auto result = m_VelocityInterpolator.Update(m_CurrentStateVel, next_state, temp_Time);
				m_CmdVel = next_state.velocity;
				m_CurrentStateVel.acceleration = next_state.acceleration;
				m_IsVelInterpolationFinished = (result == ruckig::Result::Finished);
			}
		}
		UpdatePositionCommand();
		break;

	default:
		m_CmdPos = m_FdbPos;
		m_CmdTor = 0.0;
		m_CmdVel = 0.0;
		break;
	}

	if (m_IsStandbyPosSet)
	{
		m_IsStandbyPosSet = false;
	}
}

void CAxis::ReturnToZeroPoint()
{
	Move(0.0, OpMode::CSP, kInterpolated, true);
}

bool CAxis::IsEmergency()
{
	return m_Driver.IsEmergencyState();
}

bool CAxis::IsFault()
{
	return m_Driver.IsFaultState();
}

void CAxis::ClearError()
{
	UpdatePositionCommand();
	m_Driver.ClearError();
}

void CAxis::QuickStop()
{
	m_Driver.QuickStop();
}

void CAxis::InterpolationReset(OpMode mode)
{
	if (mode == OpMode::CSP)
	{
		m_PositionInterpolator.Reset();
		m_PositionInterpolator.SetTargetState({ m_FdbPos, 0.0, 0.0 });
		m_CurrentStatePos = { m_FdbPos, m_FdbVel, 0.0 };
	}
	else if (mode == OpMode::CSV)
	{
		m_VelocityInterpolator.Reset();
		m_VelocityInterpolator.SetTargetVelocity(0.0);
		m_VelocityInterpolator.SetTargetAcceleration(0.0);
		m_CurrentStateVel = { 0.0, m_FdbVel, 0.0 }; // set the initial state
	}
}

void CAxis::UpdatePositionCommand()
{
	m_CmdPos = m_FdbPos;
}

void CAxis::CompensateAdditiveTor(double add_tor)
{
	m_Driver.SetAdditiveTorque(add_tor);
}
