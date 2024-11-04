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

void CAxisGroup::MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, ModuleAxisGroupParameter* parameters)
{
	int driver_num = 0;
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].MapParameters(inputs->AxisInputs + driver_num, outputs->AxisOutputs + driver_num, parameters->DriverParameter + driver_num, parameters->AxisGroupInterpolationParam + driver_num);
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

bool CAxisGroup::IsEnabled()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].IsEnabled())
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

void CAxisGroup::StandStill()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].StandStill();
			// Reset m_InitPos_Handwheel, prepare for entering eHandwheel state next time.
			m_InitPos_Handwheel[i] = m_Axes[i][j].m_FdbPos;
		}
	}
}

void CAxisGroup::Move(FullCommand cmd)
{
	OpMode tempMode;
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			tempMode = static_cast<OpMode>(cmd.motionCommand.MotionType[i]);
			switch (tempMode)
			{
			case OpMode::CSP:
				m_Axes[i][j].Move(cmd.motionCommand.Pos[i], OpMode::CSP, kNotInterpolated, false);
				break;
			case OpMode::CST:
				m_Axes[i][j].Move(cmd.motionCommand.Acc[i], OpMode::CST, kNotInterpolated, false);
				break;
			case OpMode::CSV:
				m_Axes[i][j].Move(cmd.motionCommand.Vel[i], OpMode::CSV, kNotInterpolated, false);
				break;
			default:
				//m_Axes[i][j].HoldPosition();
				break;
			}
		}
	}
}

void CAxisGroup::Handwheel(int axis_selected, double cmd[5])
{
	if (axis_selected >= AxisOrder::Axis_X)
	{
		for (int i = 0; i < kDriverNumPerAxis[axis_selected - 1]; i++)
		{
			m_Axes[axis_selected - 1][i].Move(cmd[axis_selected - 1]
				+ m_InitPos_Handwheel[axis_selected - 1], OpMode::CSP, kInterpolated, false);
		}
	}
}

void CAxisGroup::SingleAxisMove(AxisNum axis_index, double cmd, OpMode mode)
{
	int index = static_cast<int>(axis_index);
	for (int j = 0; j < kDriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].Move(cmd, mode, kNotInterpolated, false);
	}
}

void CAxisGroup::ReturnToZeroPoint(AxisNum axis_index)
{
	int index = static_cast<int>(axis_index);
	for (int j = 0; j < kDriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].ReturnToZeroPoint();
	}
}

void CAxisGroup::ResetInterpolator(OpMode mode)
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].InterpolationReset(mode);
		}
	}
}

AxisGroupState CAxisGroup::SafetyCheck()
{
	// Check abnormal status of axis group
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (m_Axes[i][j].IsFault())
			{
				return AxisGroupState::eAxisGroupFault;
			}
			else if (m_Axes[i][j].IsEmergency())
			{
				return AxisGroupState::eAxisGroupEmergency;
			}
			else if (m_Axes[i][j].IsExceedingLimit())
			{
				return AxisGroupState::eAxisGroupLimitViolation;
			}
		}
	}
	return AxisGroupState::eAxisGroupIdle;
}

void CAxisGroup::SwitchOpMode(OpMode mode)
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].SwitchOperationMode(mode);
		}
	}
}

bool CAxisGroup::IsOpModeSwitched()
{
	for (int i = 0; i < kActualAxisNum; i++)
	{
		for (int j = 0; j < kDriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].IsOpModeSwitched())
			{
				return false;
			}
		}
	}
	return true;
}
