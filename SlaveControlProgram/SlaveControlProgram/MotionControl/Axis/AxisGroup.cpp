#pragma once
#pragma hdrstop

#include "AxisGroup.h"

CAxisGroup::CAxisGroup()
{

}

CAxisGroup::~CAxisGroup()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].~CAxis();
		}
	}
}

void CAxisGroup::MapParameters(MotionControlInputs* _pInputs, MotionControlOutputs* _pOutputs, MotionControlParameter* _pParameters)
{
	int driver_num = 0;
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].MapParameters(_pInputs->AxisInputs + driver_num, _pOutputs->AxisOutputs + driver_num, _pParameters->DriverParameter + driver_num, _pParameters->AxisGroupInterpolationParam + driver_num);
			driver_num ++;
		}
	}
}

bool CAxisGroup::PostConstruction()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].PostConstruction())
			{
				return false;
			}
		}
	}
	return true;
}

bool CAxisGroup::Initialize()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].Initialize())
			{
				return false;
			}
		}
	}
	return true;
}

void CAxisGroup::Input()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].Input();
		}
		// Information of the first motor is used to represent the entire axis by default
		m_FdbPos[i] = m_Axes[i][0].m_FdbPos;
		m_FdbVel[i] = m_Axes[i][0].m_FdbVel;
		m_FdbTor[i] = m_Axes[i][0].m_FdbTor;
		m_CurrentOpMode[i] = m_Axes[i][0].m_ActualOpMode;
	}

	m_GantryDeviation = m_Axes[0][0].m_FdbPos - m_Axes[0][1].m_FdbPos;
}

void CAxisGroup::Output()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].Output();
		}
	}
}

bool CAxisGroup::Disable()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].Disable())
			{
				return false;
			}
		}
	}
	return true;
}

bool CAxisGroup::Enable()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].Enable())
			{
				return false;
			}
		}
	}
	return true;
}

void CAxisGroup::HoldPosition()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].HoldPosition();
			// Reset m_InitPos_Handwheel, prepare for entering eHandwheel state next time.
			m_InitPos_Handwheel[i] = m_Axes[i][j].m_StandbyPos;
		}
	}
}

void CAxisGroup::Move(FullCommand _cmd)
{
	OpMode tempMode;
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			tempMode = static_cast<OpMode>(_cmd.Motion_Command.MotionType[i]);
			switch (tempMode)
			{
			case OpMode::CSP:
				m_Axes[i][j].Move(_cmd.Motion_Command.Pos[i], OpMode::CSP, kNotInterpolated, false);
				break;
			case OpMode::CST:
				m_Axes[i][j].Move(_cmd.Motion_Command.Acc[i], OpMode::CST, kNotInterpolated, false);
				break;
			case OpMode::CSV:
				m_Axes[i][j].Move(_cmd.Motion_Command.Vel[i], OpMode::CSV, kNotInterpolated, false);
				break;
			default:
				//m_Axes[i][j].HoldPosition();
				break;
			}
		}
	}
}

void CAxisGroup::Handwheel(PanelInfo _panel)
{
	if (_panel.Handwheel_EnabledAxisNum > 1)
	{
		for (int i = 0; i < kDriverNumPerAxis[_panel.Handwheel_EnabledAxisNum - 1]; i++)
		{
			m_Axes[_panel.Handwheel_EnabledAxisNum - 1][i].Move(
				_panel.Handwheel_dPos[_panel.Handwheel_EnabledAxisNum - 1]
				+ m_InitPos_Handwheel[_panel.Handwheel_EnabledAxisNum - 1], OpMode::CSP, kInterpolated, true);
		}
	}
	else
	{
		// Ignore feedback of axis when implement interpolation for gantry
		for (int i = 0; i < kDriverNumPerAxis[_panel.Handwheel_EnabledAxisNum - 1]; i++)
		{
			m_Axes[_panel.Handwheel_EnabledAxisNum - 1][i].Move(
				_panel.Handwheel_dPos[_panel.Handwheel_EnabledAxisNum - 1]
				+ m_InitPos_Handwheel[_panel.Handwheel_EnabledAxisNum - 1], OpMode::CSP, kInterpolated, false);
		}
	}
}

void CAxisGroup::SingleAxisMove(AxisNum _axis_index, double _cmd, OpMode _mode)
{
	int index = static_cast<int>(_axis_index);
	for (int j = 0; j < kDriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].Move(_cmd, _mode, kNotInterpolated, false);
	}
}

void CAxisGroup::ReturnToZeroPoint(AxisNum _axis_index)
{
	int index = static_cast<int>(_axis_index);
	for (int j = 0; j < kDriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].ReturnToZeroPoint();
	}
}

SystemState CAxisGroup::SafetyCheck()
{
	// Check abnormal status of axis group
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (m_Axes[i][j].IsFault())
			{
				return SystemState::eFault;
			}
			else if (m_Axes[i][j].IsEmergency())
			{
				return SystemState::eEmergency;
			}
			else if (m_Axes[i][j].IsExceedingLimit())
			{
				return SystemState::eLimitViolation;
			}
		}
	}
	return SystemState::eIdle;
}
