#pragma once
#pragma hdrstop

#include "AxisGroup.h"

CAxisGroup::CAxisGroup()
{

}

CAxisGroup::~CAxisGroup()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].~CAxis();
		}
	}
}

void CAxisGroup::MapParameters(ModuleAxisGroupInputs* inputs, ModuleAxisGroupOutputs* outputs, const ModuleAxisGroupParameter* parameters)
{
	m_ActualAxisNum = parameters->ActualAxisNum;
	memcpy(m_DriverNumPerAxis, parameters->DriverNumPerAxis, sizeof(m_DriverNumPerAxis));

	int driver_num = 0;
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].MapParameters(inputs->AxisInputs + driver_num, outputs->AxisOutputs + driver_num, parameters->DriverParameter + driver_num, parameters->AxisGroupInterpolationParam + driver_num);
			driver_num ++;
		}
	}
}

bool CAxisGroup::PostConstruction()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
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
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
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
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].Input();
		}
	}

	m_GantryDeviation = m_Axes[0][0].m_FdbPos - m_Axes[0][1].m_FdbPos;
}

void CAxisGroup::Output()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].Output();
		}
	}
}

bool CAxisGroup::Disable()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
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
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
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
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].IsEnabled())
			{
				return false;
			}
		}
	}
	return true;
}

bool CAxisGroup::IsDisabled()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].IsDisabled())
			{
				return false;
			}
		}
	}
	return true;
}

void CAxisGroup::HoldPosition()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].HoldPosition();
			// Reset m_InitPos_Handwheel, prepare for entering eHandwheel state next time.
			m_InitPos_Handwheel[i] = m_Axes[i][j].m_StandbyPos;
		}
	}
}

void CAxisGroup::StandStill()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].ClearCompensateCommands();
			m_Axes[i][j].StandStill();
			// Reset m_InitPos_Handwheel, prepare for entering eHandwheel state next time.
			m_InitPos_Handwheel[i] = m_Axes[i][j].m_FdbPos;
		}
	}
}

void CAxisGroup::Move(FullCommand cmd)
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			switch (m_Axes[i][j].m_ActualOpMode)
			{
			case OpMode::CSP:
				m_Axes[i][j].Move(cmd.motionCommand.Pos[i], OpMode::CSP, kNotInterpolated, false);
				m_Axes[i][j].CompensateAdditiveTor(cmd.motionCommand.AdditiveTor[i]);
				//m_Axes[i][j].CompensateAdditiveVel(cmd.motionCommand.AdditiveVel[index]);
				break;
			case OpMode::CST:
				m_Axes[i][j].Move(cmd.motionCommand.Acc[i], OpMode::CST, kNotInterpolated, false);
				break;
			case OpMode::CSV:
				m_Axes[i][j].Move(cmd.motionCommand.Vel[i], OpMode::CSV, kNotInterpolated, false);
				m_Axes[i][j].CompensateAdditiveTor(cmd.motionCommand.AdditiveTor[i]);
				//m_Axes[i][j].CompensateAdditiveVel(cmd.motionCommand.AdditiveVel[index]);
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
		for (int i = 0; i < m_DriverNumPerAxis[axis_selected - 1]; i++)
		{
			m_Axes[axis_selected - 1][i].Move(cmd[axis_selected - 1]
				+ m_InitPos_Handwheel[axis_selected - 1], OpMode::CSP, kInterpolated, false);
		}
	}
}

void CAxisGroup::SingleAxisMove(AxisOrder axis_index, double cmd, OpMode mode)
{
	int index = static_cast<int>(axis_index) - 1;
	for (int j = 0; j < m_DriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].Move(cmd, mode, kNotInterpolated, false);
	}
}

void CAxisGroup::SingleAxisMove(AxisOrder axis_index, FullCommand cmd)
{
	int index = static_cast<int>(axis_index) - 1;
	for (int j = 0; j < m_DriverNumPerAxis[index]; j++)
	{
		switch (m_Axes[index][j].m_ActualOpMode)
		{
		case OpMode::CSP:
			m_Axes[index][j].Move(cmd.motionCommand.Pos[index], OpMode::CSP, kNotInterpolated, false);
			m_Axes[index][j].CompensateAdditivePos(cmd.motionCommand.AdditivePos[index]);
			m_Axes[index][j].CompensateAdditiveVel(cmd.motionCommand.AdditiveVel[index]);
			m_Axes[index][j].CompensateAdditiveTor(cmd.motionCommand.AdditiveTor[index]);
			break;

		case OpMode::CST:
			m_Axes[index][j].Move(cmd.motionCommand.Acc[index], OpMode::CST, kNotInterpolated, false);
			break;

		case OpMode::CSV:
			m_Axes[index][j].Move(cmd.motionCommand.Vel[index], OpMode::CSV, kNotInterpolated, false);
			m_Axes[index][j].CompensateAdditiveTor(cmd.motionCommand.AdditiveTor[index]);
			//m_Axes[index][j].CompensateAdditiveVel(cmd.motionCommand.AdditiveVel[index]);
			break;

		default:
			//m_Axes[i][j].HoldPosition();
			break;
		}
	}
}

void CAxisGroup::Positioning(const bool axis_enabled[5], const double cmd[5])
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		if (axis_enabled[i])
		{
			for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
			{
				m_Axes[i][j].Move(cmd[i], OpMode::CSP, kInterpolated, false);
			}
		}
	}
}

void CAxisGroup::ReturnToZeroPoint(AxisOrder axis_index)
{
	int index = static_cast<int>(axis_index) - 1;
	for (int j = 0; j < m_DriverNumPerAxis[index]; j++)
	{
		m_Axes[index][j].ReturnToZeroPoint();
	}
}

void CAxisGroup::ResetInterpolator(OpMode mode)
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].InterpolationReset(mode);
		}
	}
}

void CAxisGroup::SwitchOpMode(OpMode mode)
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].SwitchOperationMode(mode);
		}
	}
}

bool CAxisGroup::IsOpModeSwitched()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			if (!m_Axes[i][j].IsOpModeSwitched())
			{
				return false;
			}
		}
	}
	return true;
}

void CAxisGroup::ClearError()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].ClearError();
		}
	}
}

void CAxisGroup::QuickStop()
{
	for (int i = 0; i < m_ActualAxisNum; i++)
	{
		for (int j = 0; j < m_DriverNumPerAxis[i]; j++)
		{
			m_Axes[i][j].QuickStop();
		}
	}
}
