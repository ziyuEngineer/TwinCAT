#include "../TcPch.h"
#pragma hdrstop
#include "SimAxisGroup.h"

CSimAxisGroup::CSimAxisGroup()
{

}

CSimAxisGroup::~CSimAxisGroup()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		m_Axes[i].~CSimSERCOS();
	}
}

void CSimAxisGroup::MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters)
{
	m_ActualDriverNum = parameters->ActualAxisGroupDriverNum;
	m_CycleTime = parameters->CycleTime;

	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		m_Axes[i].MapParameters(outputs->SimAxisGroupTransmit + i, inputs->SimAxisGroupReceive + i, parameters->AxisGroupParam + i, m_CycleTime);
	}
}

void CSimAxisGroup::Initialize()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		m_Axes[i].Initialize();
	}
}

void CSimAxisGroup::Run()
{
	for (int i = 0; i < m_ActualDriverNum; i++)
	{
		m_Axes[i].Receive();
		m_Axes[i].ReactionsToReceivedCommands();
		m_Axes[i].Transmit();
	}
}