#pragma once
#pragma hdrstop

#include "Axis.h"

void CAxis::MapParameters(DriverInput* _DriverInput, DriverOutput* _DriverOutput, MotionControlInfo* _DriverParam, InterpolationParameter* _InterpolationParameter)
{
	m_Driver.MapParameters(_DriverInput, _DriverOutput, _DriverParam);
	m_InterpolationParam = _InterpolationParameter;
}

bool CAxis::PostConstruction()
{
	bool bPostConstruction = false;

	bPostConstruction = (m_InterpolationParam->CycleTime > kEpsilon && fabs_(m_InterpolationParam->MaxVelocity) > kEpsilon
		&& fabs_(m_InterpolationParam->MaxAcceleration) > kEpsilon && fabs_(m_InterpolationParam->MaxJerk) > kEpsilon);

	m_PositionInterpolator.SetCycleTime(m_InterpolationParam->CycleTime); // in [s]
	m_PositionInterpolator.SetMaxVelocity(m_InterpolationParam->MaxVelocity);
	m_PositionInterpolator.SetMaxAcceleration(m_InterpolationParam->MaxAcceleration);
	m_PositionInterpolator.SetMaxJerk(m_InterpolationParam->MaxJerk);

	m_VelocityInterpolator.SetCycleTime(m_InterpolationParam->CycleTime);
	m_VelocityInterpolator.SetMaxVelocity(m_InterpolationParam->MaxVelocity);
	m_VelocityInterpolator.SetMaxAcceleration(m_InterpolationParam->MaxAcceleration);
	m_VelocityInterpolator.SetMaxJerk(m_InterpolationParam->MaxJerk);

	return bPostConstruction;
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
}

void CAxis::Output()
{
	switch (m_Driver.m_OperationMode)
	{
	case OpMode::CSP:
		m_Driver.SetTargetPosition(m_CmdPos);
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


void CAxis::HoldPosition()
{
	if (!m_bStandbyPosSet)
	{
		m_StandbyPos = m_FdbPos;
		m_bStandbyPosSet = true;
	}
	else 
	{
		m_Driver.SetOperationMode(OpMode::CSP);
		m_CmdPos = m_StandbyPos;
	}
}

void CAxis::Move(double _cmd, OpMode _mode, bool _bInterpolated, bool _bUpdateFeedback)
{
	m_Driver.SetOperationMode(_mode);

	switch (_mode)
	{
	case OpMode::CSP:
		if (!_bInterpolated)
		{
			m_CmdPos = _cmd;
		}
		else
		{
			if (m_LastOpMode != OpMode::CSP)
			{
				InterpolationReset(OpMode::CSP);
				m_LastOpMode = OpMode::CSP;
			}

			m_CmdPosBeforeInterpolated = _cmd;
			if (_bUpdateFeedback)
			{
				m_CurrentStatePos.position = m_FdbPosFiltered;
				m_CurrentStatePos.velocity = m_FdbVelFiltered;
			}

			m_PositionInterpolator.SetTargetState({ m_CmdPosBeforeInterpolated, 0.0, 0.0 });

			if ((fabs_(_cmd - m_CmdPos) < m_InterpolationParam->Tolerance))
			{
				m_CmdPos = _cmd;
				m_bPosInterpolationFinished = false;
			}
			else if (!m_bPosInterpolationFinished)
			{
				PositionInterpolator1D::State next_state;
				double temp_Time;
				const auto result = m_PositionInterpolator.Update(m_CurrentStatePos, next_state, temp_Time);
				m_CmdPos = next_state.position;
				//m_CurrentStatePos.acceleration = next_state.acceleration;
				m_CurrentStatePos = next_state;

				m_bPosInterpolationFinished = (result == ruckig::Result::Finished);
			}
		}
		break;

	case OpMode::CST:
		m_CmdTor = _cmd;
		m_LastOpMode = OpMode::CST;
		break;

	case OpMode::CSV:
		if (!_bInterpolated)
		{
			m_CmdVel = _cmd;
		}
		else
		{
			if (m_LastOpMode != OpMode::CSV)
			{
				InterpolationReset(OpMode::CSV);
				m_LastOpMode = OpMode::CSV;
			}

			m_CmdVelBeforeInterpolated = _cmd;
			m_VelocityInterpolator.SetTargetState({ 0.0, m_CmdVelBeforeInterpolated, 0.0 });
			m_CurrentStateVel.velocity = m_FdbVelFiltered;

			if ((fabs_(_cmd - m_CmdVel) < m_InterpolationParam->Tolerance))
			{
				m_CmdVel = _cmd;
				m_bVelInterpolationFinished = false; // reset for next interpolation
			}
			else if(!m_bVelInterpolationFinished)
			{
				VelocityInterpolator1D::State next_state;
				double temp_Time;
				const auto result = m_VelocityInterpolator.Update(m_CurrentStateVel, next_state, temp_Time);
				m_CmdVel = next_state.velocity;
				m_CurrentStateVel.acceleration = next_state.acceleration;
				m_bVelInterpolationFinished = (result == ruckig::Result::Finished);
			}
		}
		break;

	default:
		m_CmdPos = m_FdbPos;
		m_CmdTor = 0.0;
		m_CmdVel = 0.0;
		break;
	}

	if (m_bStandbyPosSet)
	{
		m_bStandbyPosSet = false;
	}
}

void CAxis::ReturnToZeroPoint()
{
	Move(0.0, OpMode::CSP, kInterpolated, true);
}

bool CAxis::IsExceedingLimit()
{
	return m_Driver.IsLimitExceeded();
}

bool CAxis::IsEmergency()
{
	return m_Driver.IsEmergencyState();
}

bool CAxis::IsFault()
{
	return m_Driver.IsFaultState();
}

void CAxis::InterpolationReset(OpMode _mode)
{
	if (_mode == OpMode::CSP)
	{
		m_PositionInterpolator.Reset();
		m_PositionInterpolator.SetTargetState({ m_FdbPos, 0.0, 0.0 });
		m_CurrentStatePos = { m_FdbPos, m_FdbVel, 0.0 };
	}
	else if (_mode == OpMode::CSV)
	{
		m_VelocityInterpolator.Reset();
		m_VelocityInterpolator.SetTargetVelocity(0.0);
		m_VelocityInterpolator.SetTargetAcceleration(0.0);
		m_CurrentStateVel = { 0.0, m_FdbVel, 0.0 }; // set the initial state
	}
}
