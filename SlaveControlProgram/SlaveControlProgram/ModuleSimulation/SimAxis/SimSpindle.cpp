#include "../TcPch.h"
#pragma hdrstop
#include "SimSpindle.h"

CSimSpindle::CSimSpindle()
{

}

CSimSpindle::~CSimSpindle()
{
	m_Axis.~CSimCANopen();
}

void CSimSpindle::MapParameters(ModuleSimulationOutputs* outputs, ModuleSimulationInputs* inputs, const ModuleSimulationParameter* parameters)
{
	m_CycleTime = parameters->CycleTime;
	m_Axis.MapParameters(&outputs->SimSpindleTransmit, &inputs->SimSpindleReceive, &parameters->SpindleParam, m_CycleTime);
}

void CSimSpindle::Initialize()
{
	m_Axis.Initialize();
}

void CSimSpindle::Run()
{
	m_Axis.Receive();
	m_Axis.ReactionsToReceivedCommands();
	m_Axis.Transmit();
}